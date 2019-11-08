/*---------------------------------------
   BOUNCE.C -- Bouncing Ball Program
               (c) Charles Petzold, 1992
  ---------------------------------------*/

#include <windows.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "Bounce" ;
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
          wndclass.hIcon         = NULL ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Bouncing Ball",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     if (!SetTimer (hwnd, 1, 50, NULL))
          {
          MessageBox (hwnd, "Too many clocks or timers!",
                      szAppName, MB_ICONEXCLAMATION | MB_OK) ;
          return FALSE ;
          }

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
     static HANDLE hBitmap ;
     static short  cxClient, cyClient, xCenter, yCenter, cxTotal, cyTotal,
                   cxRadius, cyRadius, cxMove, cyMove, xPixel, yPixel ;
     HBRUSH        hBrush ;
     HDC           hdc, hdcMem ;
     short         nScale ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               xPixel = GetDeviceCaps (hdc, ASPECTX) ;
               yPixel = GetDeviceCaps (hdc, ASPECTY) ;
               ReleaseDC (hwnd, hdc) ;
               return 0 ;

          case WM_SIZE:
               xCenter = (cxClient = LOWORD (lParam)) / 2 ;
               yCenter = (cyClient = HIWORD (lParam)) / 2 ;

               nScale = min (cxClient * xPixel, cyClient * yPixel) / 16 ;

               cxRadius = nScale / xPixel ;
               cyRadius = nScale / yPixel ;

               cxMove = max (1, cxRadius / 4) ;
               cyMove = max (1, cyRadius / 4) ;

               cxTotal = 2 * (cxRadius + cxMove) ;
               cyTotal = 2 * (cyRadius + cyMove) ;

               if (hBitmap)
                    DeleteObject (hBitmap) ;

               hdc = GetDC (hwnd) ;
               hdcMem = CreateCompatibleDC (hdc) ;
               hBitmap = CreateCompatibleBitmap (hdc, cxTotal, cyTotal) ;
               ReleaseDC (hwnd, hdc) ;

               SelectObject (hdcMem, hBitmap) ;
               Rectangle (hdcMem, -1, -1, cxTotal + 1, cyTotal + 1) ;

               hBrush = CreateHatchBrush (HS_DIAGCROSS, 0L) ;
               SelectObject (hdcMem, hBrush) ;
               SetBkColor (hdcMem, RGB (255, 0, 255)) ;
               Ellipse (hdcMem, cxMove, cyMove, cxTotal - cxMove,
                                                cyTotal - cyMove) ;
               DeleteDC (hdcMem) ;
               DeleteObject (hBrush) ;
               return 0 ;

          case WM_TIMER:
               if (!hBitmap)
                    break ;

               hdc = GetDC (hwnd) ;
               hdcMem = CreateCompatibleDC (hdc) ;
               SelectObject (hdcMem, hBitmap) ;

               BitBlt (hdc, xCenter - cxTotal / 2,
                            yCenter - cyTotal / 2, cxTotal, cyTotal,
                       hdcMem, 0, 0, SRCCOPY) ;

               ReleaseDC (hwnd, hdc) ;
               DeleteDC (hdcMem) ;

               xCenter += cxMove ;
               yCenter += cyMove ;

               if ((xCenter + cxRadius >= cxClient) ||
                   (xCenter - cxRadius <= 0))
                         cxMove = -cxMove ;

               if ((yCenter + cyRadius >= cyClient) ||
                   (yCenter - cyRadius <= 0))
                         cyMove = -cyMove ;
               return 0 ;

          case WM_DESTROY:
               if (hBitmap)
                    DeleteObject (hBitmap) ;

               KillTimer (hwnd, 1) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
