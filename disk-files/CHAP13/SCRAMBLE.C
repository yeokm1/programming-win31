/*------------------------------------------------
   SCRAMBLE.C -- Scramble (and Unscramble) Screen
                 (c) Charles Petzold, 1992
  ------------------------------------------------*/

#include <windows.h>
#include <stdlib.h>

#define NUM 200

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static short nKeep [NUM][4] ;
     HDC          hdc     = CreateDC ("DISPLAY", NULL, NULL, NULL) ;
     HDC          hdcMem  = CreateCompatibleDC (hdc) ;
     short        cxSize  = GetSystemMetrics (SM_CXSCREEN) / 10 ;
     short        cySize  = GetSystemMetrics (SM_CYSCREEN) / 10 ;
     HBITMAP      hBitmap = CreateCompatibleBitmap (hdc, cxSize, cySize) ;
     short        i, j, x1, y1, x2, y2 ;

     SelectObject (hdcMem, hBitmap) ;

     srand (LOWORD (GetCurrentTime ())) ;

     for (i = 0 ; i < 2 ; i++)
          for (j = 0 ; j < NUM ; j++)
               {
               if (i == 0)
                    {
                    nKeep [j] [0] = x1 = cxSize * (rand () % 10) ;
                    nKeep [j] [1] = y1 = cySize * (rand () % 10) ;
                    nKeep [j] [2] = x2 = cxSize * (rand () % 10) ;
                    nKeep [j] [3] = y2 = cySize * (rand () % 10) ;
                    }
               else
                    {
                    x1 = nKeep [NUM - 1 - j] [0] ;
                    y1 = nKeep [NUM - 1 - j] [1] ;
                    x2 = nKeep [NUM - 1 - j] [2] ;
                    y2 = nKeep [NUM - 1 - j] [3] ;
                    }
               BitBlt (hdcMem, 0, 0, cxSize, cySize, hdc,  x1, y1, SRCCOPY) ;
               BitBlt (hdc,  x1, y1, cxSize, cySize, hdc,  x2, y2, SRCCOPY) ;
               BitBlt (hdc,  x2, y2, cxSize, cySize, hdcMem, 0, 0, SRCCOPY) ;
               }

     DeleteDC (hdcMem) ;
     DeleteDC (hdc) ;
     DeleteObject (hBitmap) ;

     return FALSE ;
     }
