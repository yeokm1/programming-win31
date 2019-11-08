/*------------------------------------------
   ROP2LOOK.C -- ROP2 Demonstration Program
                 (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Rop2Look" ;
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
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "ROP2 Demonstration Program",
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
     static LOGPEN lpBlack = { PS_SOLID, 1, 1, RGB (  0,   0,   0) },
                   lpWhite = { PS_SOLID, 1, 1, RGB (255, 255, 255) } ;
     static short  nDrawingMode = R2_COPYPEN ;
     HDC           hdc ;
     HMENU         hMenu ; 
     HPEN          hPenBlack, hPenWhite ;
     PAINTSTRUCT   ps ;
     RECT          rect ;
     short         i ;

     switch (message)
          {
          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;
               CheckMenuItem (hMenu, nDrawingMode, MF_UNCHECKED) ;
               nDrawingMode = wParam ;
               CheckMenuItem (hMenu, nDrawingMode, MF_CHECKED) ;
               InvalidateRect (hwnd, NULL, FALSE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               hPenBlack = CreatePenIndirect (&lpBlack) ;
               hPenWhite = CreatePenIndirect (&lpWhite) ;

               SetMapMode (hdc, MM_ANISOTROPIC) ;
               GetClientRect (hwnd, &rect) ;
               SetViewportExt (hdc, rect.right, rect.bottom) ;
               SetWindowExt (hdc, 10, 4) ;

               for (i = 0 ; i < 10 ; i += 2)
                    {
                    SetRect (&rect, i, 0, i + 2, 4) ;
                    FillRect (hdc, &rect, GetStockObject (i / 2)) ;
                    }
               SetROP2 (hdc, nDrawingMode) ;

               SelectObject (hdc, hPenWhite) ;
               MoveTo (hdc, 1, 1) ;
               LineTo (hdc, 9, 1) ;

               SelectObject (hdc, hPenBlack) ;
               MoveTo (hdc, 1, 3) ;
               LineTo (hdc, 9, 3) ;

               EndPaint (hwnd, &ps) ;

               DeleteObject (hPenBlack) ;
               DeleteObject (hPenWhite) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
