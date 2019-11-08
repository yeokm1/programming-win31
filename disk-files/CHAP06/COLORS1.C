/*----------------------------------------
   COLORS1.C -- Colors Using Scroll Bars
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc    (HWND, UINT, UINT, LONG) ;
long FAR PASCAL _export ScrollProc (HWND, UINT, UINT, LONG) ;

FARPROC lpfnOldScr[3] ;
HWND    hwndScrol[3], hwndLabel[3], hwndValue[3], hwndRect ;
short   color[3], nFocus ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Colors1" ;
     static char *szColorLabel[] = { "Red", "Green", "Blue" } ;
     FARPROC     lpfnScrollProc ;
     HWND        hwnd ;
     MSG         msg;
     short       n ;
     WNDCLASS    wndclass ;

     if (hPrevInstance)
          return FALSE ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = CreateSolidBrush (0L) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     RegisterClass (&wndclass) ;

     hwnd = CreateWindow (szAppName, "Color Scroll",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     hwndRect = CreateWindow ("static", NULL,
                              WS_CHILD | WS_VISIBLE | SS_WHITERECT,
                              0, 0, 0, 0,
                              hwnd, 9, hInstance, NULL) ;

     lpfnScrollProc = MakeProcInstance ((FARPROC) ScrollProc, hInstance) ;

     for (n = 0 ; n < 3 ; n++) 
          {
          hwndScrol[n] = CreateWindow ("scrollbar", NULL,
                              WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
                              0, 0, 0, 0,
                              hwnd, n, hInstance, NULL) ;

          hwndLabel[n] = CreateWindow ("static", szColorLabel[n],
                              WS_CHILD | WS_VISIBLE | SS_CENTER,
                              0, 0, 0, 0,
                              hwnd, n + 3, hInstance, NULL) ;

          hwndValue[n] = CreateWindow ("static", "0",
                              WS_CHILD | WS_VISIBLE | SS_CENTER,
                              0, 0, 0, 0,
                              hwnd, n + 6, hInstance, NULL) ; 

          lpfnOldScr[n] = (FARPROC) GetWindowLong (hwndScrol[n], GWL_WNDPROC) ;
          SetWindowLong (hwndScrol[n], GWL_WNDPROC, (LONG) lpfnScrollProc) ;

          SetScrollRange (hwndScrol[n], SB_CTL, 0, 255, FALSE) ;
          SetScrollPos   (hwndScrol[n], SB_CTL, 0, FALSE) ;
          }

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd);

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage  (&msg) ;
          }
     return msg.wParam ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static HBRUSH hBrush[3] ;
     char          szbuffer[10] ;
     HDC           hdc ;
     POINT         point ;
     short         n, cxClient, cyClient, cyChar ;
     TEXTMETRIC    tm ;

     switch (message)
          {
          case WM_CREATE :
               hBrush[0] = CreateSolidBrush (RGB (255, 0, 0)) ;
               hBrush[1] = CreateSolidBrush (RGB (0, 255, 0)) ;
               hBrush[2] = CreateSolidBrush (RGB (0, 0, 255)) ;
               return 0 ;

          case WM_SIZE :
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;

               hdc = GetDC (hwnd) ;
               GetTextMetrics (hdc, &tm) ;
               cyChar = tm.tmHeight ;
               ReleaseDC (hwnd, hdc) ;

               MoveWindow (hwndRect, 0, 0, cxClient / 2, cyClient, TRUE) ;

               for (n = 0 ; n < 3 ; n++)
                    {
                    MoveWindow (hwndScrol[n],
                         (2 * n + 1) * cxClient / 14, 2 * cyChar,
                         cxClient / 14, cyClient - 4 * cyChar, TRUE) ;

                    MoveWindow (hwndLabel[n],
                         (4 * n + 1) * cxClient / 28, cyChar / 2,
                         cxClient / 7, cyChar, TRUE) ;

                    MoveWindow (hwndValue[n],
                         (4 * n + 1) * cxClient / 28, cyClient - 3 * cyChar / 2,
                         cxClient / 7, cyChar, TRUE) ;
                    }
               SetFocus (hwnd) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hwndScrol[nFocus]) ;
               return 0 ;

          case WM_VSCROLL :
               n = GetWindowWord (HIWORD (lParam), GWW_ID) ;

               switch (wParam)
                    {
                    case SB_PAGEDOWN :
                         color[n] += 15 ;
                                                  // fall through
                    case SB_LINEDOWN :
                         color[n] = min (255, color[n] + 1) ;
                         break ;

                    case SB_PAGEUP :
                         color[n] -= 15 ;
                                                  // fall through
                    case SB_LINEUP :
                         color[n] = max (0, color[n] - 1) ;
                         break ;

                    case SB_TOP:
                         color[n] = 0 ;
                         break ;

                    case SB_BOTTOM :
                         color[n] = 255 ;
                         break ;

                    case SB_THUMBPOSITION :
                    case SB_THUMBTRACK :
                         color[n] = LOWORD (lParam) ;
                         break ;

                    default :
                         break ;
                    }
               SetScrollPos  (hwndScrol[n], SB_CTL, color[n], TRUE) ;
               SetWindowText (hwndValue[n], itoa (color[n], szbuffer, 10)) ;

               DeleteObject (
                    SetClassWord (hwnd, GCW_HBRBACKGROUND,
                         CreateSolidBrush (
                              RGB (color[0], color[1], color[2])))) ;

               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_CTLCOLOR:
               if (HIWORD (lParam) == CTLCOLOR_SCROLLBAR)
                    {
                    SetBkColor (wParam, GetSysColor (COLOR_CAPTIONTEXT)) ;
                    SetTextColor (wParam, GetSysColor (COLOR_WINDOWFRAME)) ;

                    n = GetWindowWord (LOWORD (lParam), GWW_ID) ;
                    point.x = point.y = 0 ;
                    ClientToScreen (hwnd, &point) ;
                    UnrealizeObject (hBrush[n]) ;
                    SetBrushOrg (wParam, point.x, point.y) ;
                    return ((DWORD) hBrush[n]) ;
                    }
               break ;

          case WM_DESTROY:
               DeleteObject (
                    SetClassWord (hwnd, GCW_HBRBACKGROUND,
                         GetStockObject (WHITE_BRUSH))) ;

               for (n = 0 ; n < 3 ; DeleteObject (hBrush [n++])) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

long FAR PASCAL _export ScrollProc (HWND hwnd, UINT message, UINT wParam,
                                                             LONG lParam)
     {
     short n = GetWindowWord (hwnd, GWW_ID) ;

     switch (message)
          {
          case WM_KEYDOWN:
               if (wParam == VK_TAB)
                    SetFocus (hwndScrol[(n +
                         (GetKeyState (VK_SHIFT) < 0 ? 2 : 1)) % 3]) ;
               break ;

          case WM_SETFOCUS:
               nFocus = n ;
               break ;
          }
     return CallWindowProc (lpfnOldScr[n], hwnd, message, wParam, lParam) ;
     }
