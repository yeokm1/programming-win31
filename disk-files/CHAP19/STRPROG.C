/*--------------------------------------------------------
   STRPROG.C -- Program using STRLIB dynamic link library
                (c) Charles Petzold, 1992
  --------------------------------------------------------*/

#include <windows.h>
#include <string.h>
#include "strprog.h"
#include "strlib.h"
typedef unsigned int UINT ;

#define MAXLEN 32
#define WM_DATACHANGE WM_USER

typedef struct
     {
     HDC   hdc ;
     short xText ;
     short yText ;
     short xStart ;
     short yStart ;
     short xIncr ;
     short yIncr ;
     short xMax ;
     short yMax ;
     }
     CBPARM ;

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

char szAppName [] = "StrProg" ;
char szString  [MAXLEN] ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "DLL Demonstration Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

BOOL FAR PASCAL _export DlgProc (HWND hDlg, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               SendDlgItemMessage (hDlg, IDD_STRING, EM_LIMITTEXT,
                                   MAXLEN - 1, 0L);
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                         GetDlgItemText (hDlg, IDD_STRING, szString, MAXLEN) ;
                         EndDialog (hDlg, TRUE) ;
                         return TRUE ;

                    case IDCANCEL:
                         EndDialog (hDlg, FALSE) ;
                         return TRUE ;
                    }
          }
     return FALSE ;
     }

BOOL FAR PASCAL _export EnumCallBack (HWND hwnd, LONG lParam)
     {
     char szClassName [16] ;

     GetClassName (hwnd, szClassName, sizeof szClassName) ;

     if (0 == strcmp (szClassName, szAppName))
          SendMessage (hwnd, WM_DATACHANGE, 0, 0L) ;

     return TRUE ;
     }

BOOL FAR PASCAL _export GetStrCallBack (LPSTR lpString, CBPARM FAR *lpcbp)
     {
     TextOut (lpcbp->hdc, lpcbp->xText, lpcbp->yText,
              lpString, lstrlen (lpString)) ;

     if ((lpcbp->yText += lpcbp->yIncr) > lpcbp->yMax)
          {
          lpcbp->yText = lpcbp->yStart ;
          if ((lpcbp->xText += lpcbp->xIncr) > lpcbp->xMax)
               return FALSE ;
          }
     return TRUE ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static FARPROC lpfnDlgProc, lpfnEnumCallBack ;
     static FPSTRCB lpfnGetStrCallBack ;
     static HANDLE  hInst ;
     static short   cxChar, cyChar, cxClient, cyClient ;
     CBPARM         cbparam ;
     HDC            hdc ;
     PAINTSTRUCT    ps ;
     TEXTMETRIC     tm ;

     switch (message)
          {
          case WM_CREATE:
               hInst = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnDlgProc = MakeProcInstance ((FARPROC) DlgProc, hInst) ;
               lpfnGetStrCallBack = (FPSTRCB)
                         MakeProcInstance ((FARPROC) GetStrCallBack, hInst) ;
               lpfnEnumCallBack = MakeProcInstance ((FARPROC) EnumCallBack,
                                                    hInst) ;

               hdc = GetDC (hwnd) ;
               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;
               ReleaseDC (hwnd, hdc) ;

               return 0 ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDM_ENTER:
                         if (DialogBox (hInst, "EnterDlg", hwnd, lpfnDlgProc))
                              {
                              if (AddString (szString))
                                   EnumWindows (lpfnEnumCallBack, 0L) ;
                              else
                                   MessageBeep (0) ;
                              }
                         break ;

                    case IDM_DELETE:
                         if (DialogBox (hInst, "DeleteDlg", hwnd, lpfnDlgProc))
                              {
                              if (DeleteString (szString))
                                   EnumWindows (lpfnEnumCallBack, 0L) ;
                              else
                                   MessageBeep (0) ;
                              }
                         break ;
                    }
               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_DATACHANGE:
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               cbparam.hdc   = hdc ;
               cbparam.xText = cbparam.xStart = cxChar ;
               cbparam.yText = cbparam.yStart = cyChar ;
               cbparam.xIncr = cxChar * MAXLEN ;
               cbparam.yIncr = cyChar ;
               cbparam.xMax  = cbparam.xIncr * (1 + cxClient / cbparam.xIncr) ;
               cbparam.yMax  = cyChar * (cyClient / cyChar - 1) ;

               GetStrings (lpfnGetStrCallBack, &cbparam) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
