/*-------------------------------------------------
   POEPOEM.C -- Demonstrates User-Defined Resource
                (c) Charles Petzold, 1992
  -------------------------------------------------*/

#include <windows.h>
#include "poepoem.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

char   szAppName [10] ;
char   szCaption [35] ;
HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          LoadString (hInstance, IDS_APPNAME, szAppName, sizeof szAppName) ;
          LoadString (hInstance, IDS_CAPTION, szCaption, sizeof szCaption) ;

          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }
     else
          {
          GetInstanceData (hPrevInstance, (PBYTE) szAppName, sizeof szAppName);
          GetInstanceData (hPrevInstance, (PBYTE) szCaption, sizeof szCaption);
          }

     hInst = hInstance ;

     hwnd = CreateWindow (szAppName, szCaption,
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
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

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static HANDLE hResource ;
     static HWND   hScroll ;
     static short  nPosition, cxChar, cyChar, cyClient, nNumLines, xScroll ;
     char          szPoemRes [15] ;
     char far      *lpText ;
     HDC           hdc ;
     PAINTSTRUCT   ps ;
     RECT          rect ;
     TEXTMETRIC    tm ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;
               ReleaseDC (hwnd, hdc) ;

               xScroll = GetSystemMetrics (SM_CXVSCROLL) ;

               hScroll = CreateWindow ("scrollbar", NULL,
                              WS_CHILD | WS_VISIBLE | SBS_VERT,
                              0, 0, 0, 0,
                              hwnd, 1, hInst, NULL) ;

               LoadString (hInst, IDS_POEMRES, szPoemRes, sizeof szPoemRes) ;
               hResource = LoadResource (hInst, 
                           FindResource (hInst, szPoemRes, "TEXT")) ;

               lpText = LockResource (hResource) ;

               nNumLines = 0 ;

               while (*lpText != '\\' && *lpText != '\0')
                    {
                    if (*lpText == '\n')
                         nNumLines ++ ;
                    lpText = AnsiNext (lpText) ;
                    }
               *lpText = '\0' ;

               GlobalUnlock (hResource) ;

               SetScrollRange (hScroll, SB_CTL, 0, nNumLines, FALSE) ;
               SetScrollPos   (hScroll, SB_CTL, 0, FALSE) ;
               return 0 ;

          case WM_SIZE:
               MoveWindow (hScroll, LOWORD (lParam) - xScroll, 0,
                    xScroll, cyClient = HIWORD (lParam), TRUE) ;
               SetFocus (hwnd) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hScroll) ;
               return 0 ;

          case WM_VSCROLL:
               switch (wParam)
                    {
                    case SB_TOP:
                         nPosition = 0 ;
                         break ;
                    case SB_BOTTOM:
                         nPosition = nNumLines ;
                         break ;
                    case SB_LINEUP:
                         nPosition -= 1 ;
                         break ;
                    case SB_LINEDOWN:
                         nPosition += 1 ;
                         break ;
                    case SB_PAGEUP:
                         nPosition -= cyClient / cyChar ;
                         break ;
                    case SB_PAGEDOWN:
                         nPosition += cyClient / cyChar ;
                         break ;
                    case SB_THUMBPOSITION:
                         nPosition = LOWORD (lParam) ;
                         break ;
                    }
               nPosition = max (0, min (nPosition, nNumLines)) ;

               if (nPosition != GetScrollPos (hScroll, SB_CTL))
                    {
                    SetScrollPos (hScroll, SB_CTL, nPosition, TRUE) ;
                    InvalidateRect (hwnd, NULL, TRUE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               lpText = LockResource (hResource) ;

               GetClientRect (hwnd, &rect) ;
               rect.left += cxChar ;
               rect.top  += cyChar * (1 - nPosition) ;
               DrawText (hdc, lpText, -1, &rect, DT_EXTERNALLEADING) ;

               GlobalUnlock (hResource) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               FreeResource (hResource) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
