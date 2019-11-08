/*--------------------------------------------------
   CLOVER.C -- Clover Drawing Program using Regions
               (c) Charles Petzold, 1992
  --------------------------------------------------*/

#include <windows.h>
#include <math.h>
#define TWO_PI (2.0 * 3.14159)

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Clover" ;
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

     hwnd = CreateWindow (szAppName, "Draw a Clover",
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

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static HRGN  hRgnClip ;
     static short cxClient, cyClient ;
     double       fAngle, fRadius ;
     HCURSOR      hCursor ;
     HDC          hdc ;
     HRGN         hRgnTemp [6] ;
     PAINTSTRUCT  ps ;
     short        i ;

     switch (message)
          {
          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;

               hCursor = SetCursor (LoadCursor (NULL, IDC_WAIT)) ;
               ShowCursor (TRUE) ;

               if (hRgnClip)
                    DeleteObject (hRgnClip) ;

               hRgnTemp [0] = CreateEllipticRgn (0, cyClient / 3,
                                        cxClient / 2, 2 * cyClient / 3) ;
               hRgnTemp [1] = CreateEllipticRgn (cxClient / 2, cyClient / 3,
                                        cxClient, 2 * cyClient / 3) ;
               hRgnTemp [2] = CreateEllipticRgn (cxClient / 3, 0,
                                        2 * cxClient / 3, cyClient / 2) ;
               hRgnTemp [3] = CreateEllipticRgn (cxClient / 3, cyClient / 2,
                                        2 * cxClient / 3, cyClient) ;
               hRgnTemp [4] = CreateRectRgn (0, 0, 1, 1) ;
               hRgnTemp [5] = CreateRectRgn (0, 0, 1, 1) ;
               hRgnClip     = CreateRectRgn (0, 0, 1, 1) ;

               CombineRgn (hRgnTemp [4], hRgnTemp [0], hRgnTemp [1], RGN_OR) ;
               CombineRgn (hRgnTemp [5], hRgnTemp [2], hRgnTemp [3], RGN_OR) ;
               CombineRgn (hRgnClip,     hRgnTemp [4], hRgnTemp [5], RGN_XOR) ;

               for (i = 0 ; i < 6 ; i++)
                    DeleteObject (hRgnTemp [i]) ;

               SetCursor (hCursor) ;
               ShowCursor (FALSE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               SetViewportOrg (hdc, cxClient / 2, cyClient / 2) ;
               SelectClipRgn (hdc, hRgnClip) ;

               fRadius = hypot (cxClient / 2.0, cyClient / 2.0) ;

               for (fAngle = 0.0 ; fAngle < TWO_PI ; fAngle += TWO_PI / 360)
                    {
                    MoveTo (hdc, 0, 0) ;
                    LineTo (hdc, (short) ( fRadius * cos (fAngle) + 0.5),
                                 (short) (-fRadius * sin (fAngle) + 0.5)) ;
                    }
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               DeleteObject (hRgnClip) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
