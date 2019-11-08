/*------------------------------------------
   FREEMEM.C -- Free Memory Display Program
                (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>
#include <stdio.h>

#define ID_TIMER    1

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "FreeMem" ;
     HDC         hdc ;
     HWND        hwnd ;
     MSG         msg ;
     TEXTMETRIC  tm ;
     WNDCLASS    wndclass ;

     if (hPrevInstance) 
          return FALSE ;

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

     hwnd = CreateWindow (szAppName, "Free Memory",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     hdc = GetDC (hwnd) ;
     GetTextMetrics (hdc, &tm) ;
     ReleaseDC (hwnd, hdc) ;

     if (4 * tm.tmAveCharWidth > GetSystemMetrics (SM_CXICON) ||
               2 * tm.tmHeight > GetSystemMetrics (SM_CYICON))
          {
          MessageBox (hwnd, "Icon size too small for display!",
                      szAppName, MB_ICONEXCLAMATION | MB_OK) ;
          return FALSE ;
          }

     if (!SetTimer (hwnd, ID_TIMER, 1000, NULL))
          {
          MessageBox (hwnd, "Too many clocks or timers!",
                      szAppName, MB_ICONEXCLAMATION | MB_OK) ;
          return FALSE ;
          }

     ShowWindow (hwnd, SW_SHOWMINNOACTIVE) ;
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
     static DWORD  dwFreeMem, dwPrevMem ;
     static RECT   rect ;
     char          cBuffer [20] ;
     HDC           hdc ;
     PAINTSTRUCT   ps ;

     switch (message)
          {
          case WM_TIMER:
               dwFreeMem = GetFreeSpace (0) ;

               if (dwFreeMem != dwPrevMem)
                    InvalidateRect (hwnd, NULL, TRUE) ;

               dwPrevMem = dwFreeMem ;
               return 0 ;

          case WM_SIZE:
               GetClientRect (hwnd, &rect) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               DrawText (hdc, cBuffer,
                         sprintf (cBuffer, "%.2f megs",
                                  dwFreeMem / 1024.0 / 1024.0),
                         &rect, DT_WORDBREAK) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_QUERYOPEN:
               return 0 ;

          case WM_DESTROY:
               KillTimer (hwnd, ID_TIMER) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
