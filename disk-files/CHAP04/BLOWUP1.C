/*------------------------------------------------
   BLOWUP1.C -- Screen Capture Mouse Demo Program
               (c) Charles Petzold, 1992
  ------------------------------------------------*/

#include <windows.h>

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "BlowUp1" ;
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

     hwnd = CreateWindow (szAppName, "Blow-Up Mouse Demo",
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

void InvertBlock (HWND hwnd, POINT ptBeg, POINT ptEnd)
     {
     HDC hdc ;

     hdc = CreateDC ("DISPLAY", NULL, NULL, NULL) ;
     ClientToScreen (hwnd, &ptBeg) ;
     ClientToScreen (hwnd, &ptEnd) ;
     PatBlt (hdc, ptBeg.x, ptBeg.y, ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y,
             DSTINVERT) ;
     DeleteDC (hdc) ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static BOOL  fCapturing, fBlocking ;
     static POINT ptBeg, ptEnd ;
     HDC          hdc ;
     RECT         rect ;

     switch (message)
          {
          case WM_LBUTTONDOWN:
               if (!fCapturing)
                    {
                    fCapturing = TRUE ;
                    SetCapture (hwnd) ;
                    SetCursor (LoadCursor (NULL, IDC_CROSS)) ;
                    }
               else if (!fBlocking)
                    {
                    fBlocking = TRUE ;
                    ptBeg = MAKEPOINT (lParam) ;
                    }
               return 0 ;

          case WM_MOUSEMOVE:
               if (fBlocking)
                    {
                    ptEnd = MAKEPOINT (lParam) ;
                    InvertBlock (hwnd, ptBeg, ptEnd) ;
                    InvertBlock (hwnd, ptBeg, ptEnd) ;
                    }
               return 0 ;

          case WM_LBUTTONUP:
               if (fBlocking)
                    {
                    fCapturing = fBlocking = FALSE ;
                    ptEnd = MAKEPOINT (lParam) ;
                    SetCursor (LoadCursor (NULL, IDC_WAIT)) ;

                    hdc = GetDC (hwnd) ; 
                    GetClientRect (hwnd, &rect) ;
                    StretchBlt (hdc, 0, 0, rect.right, rect.bottom,
                                hdc, ptBeg.x, ptBeg.y,
                                ptEnd.x - ptBeg.x, ptEnd.y - ptBeg.y,
                                SRCCOPY) ;

                    ReleaseDC (hwnd, hdc) ;
                    SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
                    ReleaseCapture () ;
                    }
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
