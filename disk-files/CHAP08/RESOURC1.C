/*-----------------------------------------------------------
   RESOURC1.C -- Icon and Cursor Demonstration Program No. 1
                 (c) Charles Petzold, 1992
  -----------------------------------------------------------*/

#include <windows.h>

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

char   szAppName [] = "Resourc1" ;
HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (hInstance, szAppName) ;
          wndclass.hbrBackground = COLOR_WINDOW + 1 ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hInst = hInstance ;

     hwnd = CreateWindow (szAppName, "Icon and Cursor Demo",
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
     static HICON hIcon ;
     static short cxIcon, cyIcon, cxClient, cyClient ;
     HDC          hdc ;
     PAINTSTRUCT  ps ;
     short        x, y ;

     switch (message)
          {
          case WM_CREATE:
               hIcon = LoadIcon (hInst, szAppName) ;
               cxIcon = GetSystemMetrics (SM_CXICON) ;
               cyIcon = GetSystemMetrics (SM_CYICON) ;
               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               for (y = cyIcon ; y < cyClient ; y += 2 * cyIcon)
                    for (x = cxIcon ; x < cxClient ; x += 2 * cxIcon)
                         DrawIcon (hdc, x, y, hIcon) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
