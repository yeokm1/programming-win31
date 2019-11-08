/*-------------------------------------------------------
   KEYLOOK.C -- Displays Keyboard and Character Messages
                (c) Charles Petzold, 1992
  -------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>

long  FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

RECT  rect ;
short cxChar, cyChar ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "KeyLook" ;
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
          wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Keyboard Message Looker",
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

void ShowKey (HWND hwnd, int iType, char *szMessage, UINT wParam, LONG lParam)
     {
     static char *szFormat[2] = { "%-14s %3d    %c %6u %4d %3s %3s %4s %4s",
                                  "%-14s    %3d %c %6u %4d %3s %3s %4s %4s" } ;
     char        szBuffer[80] ;
     HDC         hdc ;

     ScrollWindow (hwnd, 0, -cyChar, &rect, &rect) ;
     hdc = GetDC (hwnd) ;

     SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

     TextOut (hdc, cxChar, rect.bottom - cyChar, szBuffer,
              wsprintf (szBuffer, szFormat [iType],
                        (LPSTR) szMessage, wParam,
                        (BYTE) (iType ? wParam : ' '),
                        LOWORD (lParam),
                        HIWORD (lParam) & 0xFF,
                        (LPSTR) (0x01000000 & lParam ? "Yes"  : "No"),
                        (LPSTR) (0x20000000 & lParam ? "Yes"  : "No"),
                        (LPSTR) (0x40000000 & lParam ? "Down" : "Up"),
                        (LPSTR) (0x80000000 & lParam ? "Up"   : "Down"))) ;

     ReleaseDC (hwnd, hdc) ;
     ValidateRect (hwnd, NULL) ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static char szTop[] =
                    "Message        Key Char Repeat Scan Ext ALT Prev Tran";
     static char szUnd[]=
                    "_______        ___ ____ ______ ____ ___ ___ ____ ____";
     HDC         hdc ;
     PAINTSTRUCT ps ;
     TEXTMETRIC  tm ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;

               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight ;

               ReleaseDC (hwnd, hdc) ;

               rect.top = 3 * cyChar / 2 ;
               return 0 ;

          case WM_SIZE:
               rect.right  = LOWORD (lParam) ;
               rect.bottom = HIWORD (lParam) ;
               UpdateWindow (hwnd) ;
               return 0 ;

          case WM_PAINT:
               InvalidateRect (hwnd, NULL, TRUE) ;
               hdc = BeginPaint (hwnd, &ps) ;

               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

               SetBkMode (hdc, TRANSPARENT) ;
               TextOut (hdc, cxChar, cyChar / 2, szTop, (sizeof szTop) - 1) ;
               TextOut (hdc, cxChar, cyChar / 2, szUnd, (sizeof szUnd) - 1) ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_KEYDOWN:
               ShowKey (hwnd, 0, "WM_KEYDOWN", wParam, lParam) ;
               return 0 ;

          case WM_KEYUP:
               ShowKey (hwnd, 0, "WM_KEYUP", wParam, lParam) ;
               return 0 ;

          case WM_CHAR:
               ShowKey (hwnd, 1, "WM_CHAR", wParam, lParam) ;
               return 0 ;

          case WM_DEADCHAR:
               ShowKey (hwnd, 1, "WM_DEADCHAR", wParam, lParam) ;
               return 0 ;

          case WM_SYSKEYDOWN:
               ShowKey (hwnd, 0, "WM_SYSKEYDOWN", wParam, lParam) ;
               break ;        // ie, call DefWindowProc

          case WM_SYSKEYUP:
               ShowKey (hwnd, 0, "WM_SYSKEYUP", wParam, lParam) ;
               break ;        // ie, call DefWindowProc

          case WM_SYSCHAR:
               ShowKey (hwnd, 1, "WM_SYSCHAR", wParam, lParam) ;
               break ;        // ie, call DefWindowProc

          case WM_SYSDEADCHAR:
               ShowKey (hwnd, 1, "WM_SYSDEADCHAR", wParam, lParam) ;
               break ;        // ie, call DefWindowProc

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
