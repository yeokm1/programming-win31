/*----------------------------------------------------
   SYSMETS3.C -- System Metrics Display Program No. 3
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
     static char szAppName[] = "SysMets3" ;
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

     hwnd = CreateWindow (szAppName, "Get System Metrics No. 3",
                          WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
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
     static short cxChar, cxCaps, cyChar, cxClient, cyClient, nMaxWidth,
                  nVscrollPos, nVscrollMax, nHscrollPos, nHscrollMax ;
     char         szBuffer[10] ;
     HDC          hdc ;
     short        i, x, y, nPaintBeg, nPaintEnd, nVscrollInc, nHscrollInc ;
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

               nMaxWidth = 40 * cxChar + 22 * cxCaps ;
               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;

               SetScrollRange (hwnd, SB_VERT, 0, nVscrollMax, FALSE) ;
               SetScrollPos   (hwnd, SB_VERT, nVscrollPos, TRUE) ;

	       nHscrollMax = max (0, 2 + (nMaxWidth - cxClient) / cxChar) ;
               nHscrollPos = min (nHscrollPos, nHscrollMax) ;

               SetScrollRange (hwnd, SB_HORZ, 0, nHscrollMax, FALSE) ;
               SetScrollPos   (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
               return 0 ;

          case WM_VSCROLL:
               switch (wParam)
                    {
                    case SB_TOP:
                         nVscrollInc = -nVscrollPos ;
                         break ;

                    case SB_BOTTOM:
                         nVscrollInc = nVscrollMax - nVscrollPos ;
                         break ;

                    case SB_LINEUP:
                         nVscrollInc = -1 ;
                         break ;

                    case SB_LINEDOWN:
                         nVscrollInc = 1 ;
                         break ;

                    case SB_PAGEUP:
                         nVscrollInc = min (-1, -cyClient / cyChar) ;
                         break ;

                    case SB_PAGEDOWN:
                         nVscrollInc = max (1, cyClient / cyChar) ;
                         break ;

                    case SB_THUMBTRACK:
                         nVscrollInc = LOWORD (lParam) - nVscrollPos ;
                         break ;

                    default:
                         nVscrollInc = 0 ;
                    }
               nVscrollInc = max (-nVscrollPos,
                             min (nVscrollInc, nVscrollMax - nVscrollPos)) ;

               if (nVscrollInc != 0)
                    {
                    nVscrollPos += nVscrollInc ;
                    ScrollWindow (hwnd, 0, -cyChar * nVscrollInc, NULL, NULL) ;
                    SetScrollPos (hwnd, SB_VERT, nVscrollPos, TRUE) ;
                    UpdateWindow (hwnd) ;
                    }
               return 0 ;

          case WM_HSCROLL:
               switch (wParam)
                    {
                    case SB_LINEUP:
                         nHscrollInc = -1 ;
                         break ;

                    case SB_LINEDOWN:
                         nHscrollInc = 1 ;
                         break ;

                    case SB_PAGEUP:
                         nHscrollInc = -8 ;
                         break ;

                    case SB_PAGEDOWN:
                         nHscrollInc = 8 ;
                         break ;

                    case SB_THUMBPOSITION:
                         nHscrollInc = LOWORD (lParam) - nHscrollPos ;
                         break ;

                    default:
                         nHscrollInc = 0 ;
                    }
               nHscrollInc = max (-nHscrollPos,
                             min (nHscrollInc, nHscrollMax - nHscrollPos)) ;

               if (nHscrollInc != 0)
                    {
                    nHscrollPos += nHscrollInc ;
                    ScrollWindow (hwnd, -cxChar * nHscrollInc, 0, NULL, NULL) ;
                    SetScrollPos (hwnd, SB_HORZ, nHscrollPos, TRUE) ;
                    }
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               nPaintBeg = max (0, nVscrollPos + ps.rcPaint.top / cyChar - 1) ;
               nPaintEnd = min (NUMLINES,
                                nVscrollPos + ps.rcPaint.bottom / cyChar) ;

               for (i = nPaintBeg ; i < nPaintEnd ; i++)
                    {
                    x = cxChar * (1 - nHscrollPos) ;
                    y = cyChar * (1 - nVscrollPos + i) ;

                    TextOut (hdc, x, y,
                             sysmetrics[i].szLabel,
			     lstrlen (sysmetrics[i].szLabel)) ;

                    TextOut (hdc, x + 22 * cxCaps, y,
                             sysmetrics[i].szDesc,
			     lstrlen (sysmetrics[i].szDesc)) ;

                    SetTextAlign (hdc, TA_RIGHT | TA_TOP) ;

                    TextOut (hdc, x + 22 * cxCaps + 40 * cxChar, y,
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
