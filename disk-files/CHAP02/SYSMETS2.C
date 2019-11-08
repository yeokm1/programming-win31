/*----------------------------------------------------
   SYSMETS2.C -- System Metrics Display Program No. 2
                 (c) Charles Petzold, 1992
  ----------------------------------------------------*/

#include <windows.h>
#include "sysmets.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "SysMets2" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

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
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Get System Metrics No. 2",
                          WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
     static short cxChar, cxCaps, cyChar, cyClient, nVscrollPos ;
     char         szBuffer[10] ;
     HDC          hdc ;
     short        i, y ;
     PAINTSTRUCT  ps ;
     TEXTMETRIC   tm ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;

               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2 ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;

               ReleaseDC (hwnd, hdc) ;

               SetScrollRange (hwnd, SB_VERT, 0, NUMLINES, FALSE) ;
               SetScrollPos   (hwnd, SB_VERT, nVscrollPos, TRUE) ;
               return 0 ;

          case WM_SIZE:
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_VSCROLL:
               switch (wParam)
                    {
                    case SB_LINEUP:
                         nVscrollPos -= 1 ;
                         break ;

                    case SB_LINEDOWN:
                         nVscrollPos += 1 ;
                         break ;

                    case SB_PAGEUP:
                         nVscrollPos -= cyClient / cyChar ;
                         break ;

                    case SB_PAGEDOWN:
                         nVscrollPos += cyClient / cyChar ;
                         break ;

                    case SB_THUMBPOSITION:
                         nVscrollPos = LOWORD (lParam) ;
                         break ;

                    default:
                         break ;
                    }
               nVscrollPos = max (0, min (nVscrollPos, NUMLINES)) ;

               if (nVscrollPos != GetScrollPos (hwnd, SB_VERT))
                    {
                    SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
                    InvalidateRect (hwnd, NULL, TRUE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               for (i = 0 ; i < NUMLINES ; i++)
                    {
                    y = cyChar * (1 - nVscrollPos + i) ;

                    TextOut (hdc, cxChar, y,
                             sysmetrics[i].szLabel,
			     lstrlen (sysmetrics[i].szLabel)) ;

                    TextOut (hdc, cxChar + 22 * cxCaps, y,
                             sysmetrics[i].szDesc,
			     lstrlen (sysmetrics[i].szDesc)) ;

                    SetTextAlign (hdc, TA_RIGHT | TA_TOP) ;

                    TextOut (hdc, cxChar + 22 * cxCaps + 40 * cxChar, y,
                             szBuffer,
                             wsprintf (szBuffer, "%5d",
				  GetSystemMetrics (sysmetrics[i].nIndex))) ;

                    SetTextAlign (hdc, TA_LEFT | TA_TOP) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }

     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
