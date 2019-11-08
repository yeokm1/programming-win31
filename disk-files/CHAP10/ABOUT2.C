/*------------------------------------------
   ABOUT2.C -- About Box Demo Program No. 2
               (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>
#include "about2.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export AboutDlgProc (HWND, UINT, UINT, LONG) ;

short nCurrentColor  = IDD_BLACK,
      nCurrentFigure = IDD_RECT ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "About2" ;
     MSG         msg;
     HWND        hwnd ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "About Box Demo Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd); 

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

void PaintWindow (HWND hwnd, short nColor, short nFigure)
     {
     static DWORD dwColor [8] = { RGB (0,     0, 0), RGB (  0,   0, 255),
                                  RGB (0,   255, 0), RGB (  0, 255, 255),
                                  RGB (255,   0, 0), RGB (255,   0, 255),
                                  RGB (255, 255, 0), RGB (255, 255, 255) } ;
     HBRUSH       hBrush ;
     HDC          hdc ;
     RECT         rect ;

     hdc = GetDC (hwnd) ;
     GetClientRect (hwnd, &rect) ;
     hBrush = CreateSolidBrush (dwColor [nColor - IDD_BLACK]) ;
     hBrush = SelectObject (hdc, hBrush) ;

     if (nFigure == IDD_RECT)
          Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom) ;
     else
          Ellipse   (hdc, rect.left, rect.top, rect.right, rect.bottom) ;

     DeleteObject (SelectObject (hdc, hBrush)) ;
     ReleaseDC (hwnd, hdc) ;
     }

void PaintTheBlock (HWND hCtrl, short nColor, short nFigure)
     {
     InvalidateRect (hCtrl, NULL, TRUE) ;
     UpdateWindow (hCtrl) ;
     PaintWindow (hCtrl, nColor, nFigure) ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static FARPROC lpfnAboutDlgProc ;
     static HANDLE  hInstance ;
     PAINTSTRUCT    ps ;

     switch (message)
          {
          case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnAboutDlgProc = MakeProcInstance ((FARPROC) AboutDlgProc,
                                                    hInstance) ;
               return 0 ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDM_ABOUT:
                         if (DialogBox (hInstance, "AboutBox", hwnd,
                                        lpfnAboutDlgProc))
                              InvalidateRect (hwnd, NULL, TRUE) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               BeginPaint (hwnd, &ps) ;
               EndPaint (hwnd, &ps) ;

               PaintWindow (hwnd, nCurrentColor, nCurrentFigure) ;
               return 0 ;
                    
          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

BOOL FAR PASCAL _export AboutDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
     {
     static HWND  hCtrlBlock ;
     static short nColor, nFigure ;

     switch (message)
          {
          case WM_INITDIALOG:
               nColor  = nCurrentColor ;
               nFigure = nCurrentFigure ;

               CheckRadioButton (hDlg, IDD_BLACK, IDD_WHITE, nColor) ; 
               CheckRadioButton (hDlg, IDD_RECT,  IDD_ELL,   nFigure) ;

               hCtrlBlock = GetDlgItem (hDlg, IDD_PAINT) ;

               SetFocus (GetDlgItem (hDlg, nColor)) ;
               return FALSE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                         nCurrentColor  = nColor ;
                         nCurrentFigure = nFigure ;
                         EndDialog (hDlg, TRUE) ;
                         return TRUE ;

                    case IDCANCEL:
                         EndDialog (hDlg, FALSE) ;
                         return TRUE ;

                    case IDD_BLACK:
                    case IDD_RED:
                    case IDD_GREEN:
                    case IDD_YELLOW:
                    case IDD_BLUE:
                    case IDD_MAGENTA:
                    case IDD_CYAN:
                    case IDD_WHITE:
                         nColor = wParam ;
                         CheckRadioButton (hDlg, IDD_BLACK, IDD_WHITE, wParam);
                         PaintTheBlock (hCtrlBlock, nColor, nFigure) ;
                         return TRUE ;

                    case IDD_RECT:
                    case IDD_ELL:
                         nFigure = wParam ;
                         CheckRadioButton (hDlg, IDD_RECT, IDD_ELL, wParam) ;
                         PaintTheBlock (hCtrlBlock, nColor, nFigure) ;
                         return TRUE ;
                    }
               break ;

          case WM_PAINT:
               PaintTheBlock (hCtrlBlock, nColor, nFigure) ;
               break ;
          }
     return FALSE ;
     }
