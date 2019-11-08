/*------------------------------------------
   RANDRECT.C -- Displays Random Rectangles
                 (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
void DrawRectangle (HWND) ;

short cxClient, cyClient ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "RandRect" ;
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

          !RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Random Rectangles",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (TRUE)
          {
          if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
               {
               if (msg.message == WM_QUIT)
                    break ;

               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          else
               DrawRectangle (hwnd) ;
          }
     return msg.wParam ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     switch (message)
          {
          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

void DrawRectangle (HWND hwnd)
     {
     HBRUSH hBrush ;
     HDC    hdc ;
     short  xLeft, xRight, yTop, yBottom, nRed, nGreen, nBlue ;

     xLeft   = rand () % cxClient ;
     xRight  = rand () % cxClient ;
     yTop    = rand () % cyClient ;
     yBottom = rand () % cyClient ;
     nRed    = rand () & 255 ;
     nGreen  = rand () & 255 ;
     nBlue   = rand () & 255 ;

     hdc = GetDC (hwnd) ;
     hBrush = CreateSolidBrush (RGB (nRed, nGreen, nBlue)) ;
     SelectObject (hdc, hBrush) ;

     Rectangle (hdc, min (xLeft, xRight), min (yTop, yBottom),
                     max (xLeft, xRight), max (yTop, yBottom)) ;

     ReleaseDC (hwnd, hdc) ;
     DeleteObject (hBrush) ;
     }     
