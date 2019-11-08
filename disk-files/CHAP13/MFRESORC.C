/*-----------------------------------------
   MFRESORC.C -- Metafile Resource Program
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "MFResorc" ;
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

     hwnd = CreateWindow (szAppName, "Metafile Resource Program",
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
     static HANDLE hmf ;
     static short  cxClient, cyClient ;
     HANDLE        hInstance, hResource ;
     HDC           hdc ;
     PAINTSTRUCT   ps ;
     short         x, y ;

     switch (message)
          {
          case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam) -> hInstance ;
               hResource = LoadResource (hInstance, 
                           FindResource (hInstance, "MyLogo", "METAFILE")) ;

               LockResource (hResource) ;
               hmf = SetMetaFileBits (hResource) ;
               UnlockResource (hResource) ;
               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               SetMapMode (hdc, MM_ANISOTROPIC) ;
               SetWindowExt (hdc, 1000, 1000) ;
               SetViewportExt (hdc, cxClient, cyClient) ;

               for (x = 0 ; x < 10 ; x++)
                    for (y = 0 ; y < 10 ; y++)
                         {
                         SetWindowOrg (hdc, -100 * x, -100 * y) ;
                         PlayMetaFile (hdc, hmf) ;
                         }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               DeleteMetaFile (hmf) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
