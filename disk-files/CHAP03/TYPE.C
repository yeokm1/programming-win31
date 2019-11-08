/*-------------------------------------
   TYPE.C -- Typing Program
             (c) Charles Petzold, 1992
  -------------------------------------*/

#include <windows.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define BUFFER(x,y) *(pBuffer + y * cxBuffer + x)

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Type" ;
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

     hwnd = CreateWindow (szAppName, "Typing Program",
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
     static char *pBuffer = NULL ;
     static int  cxChar, cyChar, cxClient, cyClient, cxBuffer, cyBuffer,
                 xCaret, yCaret ;
     HDC         hdc ;
     int         x, y, i ;
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
               return 0 ;

          case WM_SIZE:
                                   // obtain window size in pixels

               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;

                                   // calculate window size in characters

               cxBuffer = max (1, cxClient / cxChar) ;
               cyBuffer = max (1, cyClient / cyChar) ;

                                   // allocate memory for buffer and clear it

               if (pBuffer != NULL)
                    free (pBuffer) ;

               if ((LONG) cxBuffer * cyBuffer > 65535L ||
                    (pBuffer = (char *) malloc (cxBuffer * cyBuffer)) == NULL)

                         MessageBox (hwnd, "Window too large.  Cannot "
                                           "allocate enough memory.", "Type",
                                     MB_ICONEXCLAMATION | MB_OK) ;
               else
                    for (y = 0 ; y < cyBuffer ; y++)
                         for (x = 0 ; x < cxBuffer ; x++)
                              BUFFER(x,y) = ' ' ;

                                   // set caret to upper left corner
               xCaret = 0 ;
               yCaret = 0 ;

               if (hwnd == GetFocus ())
                    SetCaretPos (xCaret * cxChar, yCaret * cyChar) ;

               return 0 ;

          case WM_SETFOCUS:
                                   // create and show the caret

               CreateCaret (hwnd, NULL, cxChar, cyChar) ;
               SetCaretPos (xCaret * cxChar, yCaret * cyChar) ;
               ShowCaret (hwnd) ;
               return 0 ;

          case WM_KILLFOCUS:
                                   // hide and destroy the caret
               HideCaret (hwnd) ;
               DestroyCaret () ;
               return 0 ;

          case WM_KEYDOWN:
               switch (wParam)
                    {
                    case VK_HOME:
                         xCaret = 0 ;
                         break ;

                    case VK_END:
                         xCaret = cxBuffer - 1 ;
                         break ;

                    case VK_PRIOR:
                         yCaret = 0 ;
                         break ;

                    case VK_NEXT:
                         yCaret = cyBuffer - 1 ;
                         break ;

                    case VK_LEFT:
                         xCaret = max (xCaret - 1, 0) ;
                         break ;

                    case VK_RIGHT:
                         xCaret = min (xCaret + 1, cxBuffer - 1) ;
                         break ;

                    case VK_UP:
                         yCaret = max (yCaret - 1, 0) ;
                         break ;

                    case VK_DOWN:
                         yCaret = min (yCaret + 1, cyBuffer - 1) ;
                         break ;

                    case VK_DELETE:
                         for (x = xCaret ; x < cxBuffer - 1 ; x++)
                              BUFFER (x, yCaret) = BUFFER (x + 1, yCaret) ;

                         BUFFER (cxBuffer - 1, yCaret) = ' ' ;

                         HideCaret (hwnd) ;
                         hdc = GetDC (hwnd) ;

                         SelectObject (hdc,
                              GetStockObject (SYSTEM_FIXED_FONT)) ;

                         TextOut (hdc, xCaret * cxChar, yCaret * cyChar,
                                  & BUFFER (xCaret, yCaret),
                                  cxBuffer - xCaret) ;

                         ShowCaret (hwnd) ;
                         ReleaseDC (hwnd, hdc) ;
                         break ;
                    }

               SetCaretPos (xCaret * cxChar, yCaret * cyChar) ;
               return 0 ;

          case WM_CHAR:
               for (i = 0 ; i < (int) LOWORD (lParam) ; i++)
                    {
                    switch (wParam)
                         {
                         case '\b' :                    // backspace
                              if (xCaret > 0)
                                   {
                                   xCaret-- ;
                                   SendMessage (hwnd, WM_KEYDOWN,
                                                VK_DELETE, 1L) ;
                                   }
                              break ;

                         case '\t' :                    // tab
                              do
                                   {
                                   SendMessage (hwnd, WM_CHAR, ' ', 1L) ;
                                   }
                              while (xCaret % 8 != 0) ;
                              break ;

                         case '\n' :                    // line feed
                              if (++yCaret == cyBuffer)
                                   yCaret = 0 ;
                              break ;

                         case '\r' :                    // carriage return
                              xCaret = 0 ;

                              if (++yCaret == cyBuffer)
                                   yCaret = 0 ;
                              break ;

                         case '\x1B' :                  // escape
                              for (y = 0 ; y < cyBuffer ; y++)
                                   for (x = 0 ; x < cxBuffer ; x++)
                                        BUFFER (x, y) = ' ' ;

                              xCaret = 0 ;
                              yCaret = 0 ;

                              InvalidateRect (hwnd, NULL, FALSE) ;
                              break ;

                         default:                       // character codes
                              BUFFER (xCaret, yCaret) = (char) wParam ;

                              HideCaret (hwnd) ;
                              hdc = GetDC (hwnd) ;

                              SelectObject (hdc,
                                   GetStockObject (SYSTEM_FIXED_FONT)) ;

                              TextOut (hdc, xCaret * cxChar, yCaret * cyChar,
                                       & BUFFER (xCaret, yCaret), 1) ;

                              ShowCaret (hwnd) ;
                              ReleaseDC (hwnd, hdc) ;

                              if (++xCaret == cxBuffer)
                                   {
                                   xCaret = 0 ;

                                   if (++yCaret == cyBuffer)
                                        yCaret = 0 ;
                                   }
                              break ;
                         }
                    }

               SetCaretPos (xCaret * cxChar, yCaret * cyChar) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

               for (y = 0 ; y < cyBuffer ; y++)
                    TextOut (hdc, 0, y * cyChar, & BUFFER(0,y), cxBuffer) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
