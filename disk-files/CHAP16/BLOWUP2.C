/*------------------------------------------------
   BLOWUP2.C -- Capture Screen Image to Clipboard
                (c) Charles Petzold, 1992
  ------------------------------------------------*/

#include <windows.h>
#include <stdlib.h>
typedef unsigned int UINT ;

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "Blowup2" ;
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

     hwnd = CreateWindow (szAppName, szAppName, 
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

void InvertBlock (HWND hwnd, POINT org, POINT len)
     {
     HDC   hdc ;

     hdc = CreateDC ("DISPLAY", NULL, NULL, NULL) ;
     ClientToScreen (hwnd, &org) ;
     PatBlt (hdc, org.x, org.y, len.x, len.y, DSTINVERT) ;
     DeleteDC (hdc) ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static BOOL  bCapturing, bBlocking ;
     static POINT org, len ;
     static short cxClient, cyClient ;
     BITMAP       bm ;
     HDC          hdc, hdcMem ;
     HBITMAP      hBitmap ;
     PAINTSTRUCT  ps ;

     switch (message)
          {
          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_LBUTTONDOWN:
               if (!bCapturing)
                    {
                    bCapturing = TRUE ;
                    SetCapture (hwnd) ;
                    SetCursor (LoadCursor (NULL, IDC_CROSS)) ;
                    }
               else if (!bBlocking)
                    {
                    bBlocking = TRUE ;
                    org = MAKEPOINT (lParam) ;
                    }
               return 0 ;

          case WM_MOUSEMOVE:
               if (bCapturing)
                    SetCursor (LoadCursor (NULL, IDC_CROSS)) ;

               if (bBlocking)
                    {
                    len = MAKEPOINT (lParam) ;
                    len.x -= org.x ;
                    len.y -= org.y ;

                    InvertBlock (hwnd, org, len) ;
                    InvertBlock (hwnd, org, len) ;
                    }
               return 0 ;

          case WM_LBUTTONUP:
               if (!bBlocking)
                    break ;

               bCapturing = bBlocking = FALSE ;
               SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
               ReleaseCapture () ;

               if (len.x == 0 || len.y == 0)
                    break ;

               hdc = GetDC (hwnd) ;
               hdcMem = CreateCompatibleDC (hdc) ;
               hBitmap = CreateCompatibleBitmap (hdc,
                                    abs (len.x), abs (len.y)) ;
               if (hBitmap)
                    {
                    SelectObject (hdcMem, hBitmap) ;
                    StretchBlt (hdcMem, 0, 0, abs (len.x), abs (len.y),
                         hdc, org.x, org.y, len.x, len.y, SRCCOPY) ;

                    OpenClipboard (hwnd) ;
                    EmptyClipboard () ;
                    SetClipboardData (CF_BITMAP, hBitmap) ;
                    CloseClipboard () ;

                    InvalidateRect (hwnd, NULL, TRUE) ;
                    }
               else
                    MessageBeep (0) ;

               DeleteDC (hdcMem) ;
               ReleaseDC (hwnd, hdc) ;
               return 0 ;

          case WM_PAINT:
               InvalidateRect (hwnd, NULL, TRUE) ;
               hdc = BeginPaint (hwnd, &ps) ;
               OpenClipboard (hwnd) ;

               hBitmap = GetClipboardData (CF_BITMAP) ;

               if (hBitmap != NULL)
                    {
                    SetCursor (LoadCursor (NULL, IDC_WAIT)) ;

                    hdcMem = CreateCompatibleDC (hdc) ;
                    SelectObject (hdcMem, hBitmap) ;
                    GetObject (hBitmap, sizeof (BITMAP), (LPSTR) &bm) ;

                    SetStretchBltMode (hdc, COLORONCOLOR) ;
                    StretchBlt (hdc,    0, 0, cxClient,   cyClient,
                                hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,
                                                  SRCCOPY) ;

                    SetCursor (LoadCursor (NULL, IDC_ARROW)) ;
                    DeleteDC (hdcMem) ;
                    }

               CloseClipboard () ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
