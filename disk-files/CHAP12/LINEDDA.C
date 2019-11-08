/*----------------------------------------
   LINEDDA.C -- LineDDA Demonstration
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>

long FAR PASCAL _export WndProc  (HWND, UINT, UINT, LONG) ;
void FAR PASCAL _export LineProc (int, int, LONG) ;

HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "LineDDA" ;
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

     hInst = hInstance ;

     hwnd = CreateWindow (szAppName, "LineDDA Demonstration",
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
     static FARPROC lpfnLineProc ;
     static short   cxClient, cyClient, xL, xR, yT, yB ;
     HDC            hdc ;
     PAINTSTRUCT    ps ;

     switch (message)
          {
          case WM_CREATE:
               lpfnLineProc = MakeProcInstance ((FARPROC) LineProc, hInst) ;
               return 0 ;

          case WM_SIZE:
               xR = 3 * (xL = (cxClient = LOWORD (lParam)) / 4) ;
               yB = 3 * (yT = (cyClient = HIWORD (lParam)) / 4) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               LineDDA (xL, yT, xR, yT, lpfnLineProc, (LONG) hdc) ;
               LineDDA (xR, yT, xR, yB, lpfnLineProc, (LONG) hdc) ;
               LineDDA (xR, yB, xL, yB, lpfnLineProc, (LONG) hdc) ;
               LineDDA (xL, yB, xL, yT, lpfnLineProc, (LONG) hdc) ;

               LineDDA (0,        0,        xL, yT, lpfnLineProc, (LONG) hdc) ;
               LineDDA (cxClient, 0,        xR, yT, lpfnLineProc, (LONG) hdc) ;
               LineDDA (cxClient, cyClient, xR, yB, lpfnLineProc, (LONG) hdc) ;
               LineDDA (0,        cyClient, xL, yB, lpfnLineProc, (LONG) hdc) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

void FAR PASCAL _export LineProc (int x, int y, LONG lData)
     {
     static short nCounter = 0 ;

     if (nCounter == 2)
          Ellipse ((HDC) lData, x - 2, y - 2, x + 3, y + 3) ;

     nCounter = (nCounter + 1) % 4 ;
     }
