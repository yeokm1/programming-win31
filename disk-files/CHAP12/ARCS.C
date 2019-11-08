/*-------------------------------------------------------
   ARCS.C -- Demonstrates Drawing Arcs, Chords, and Pies
             (c) Charles Petzold, 1992
  -------------------------------------------------------*/

#include <windows.h>
#include "arcs.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Arcs" ;
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

     hwnd = CreateWindow (szAppName, "Arcs, Chords, and Pies",
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
     static short cxClient, cyClient, x1, x2, x3, x4, y1, y2, y3, y4,
                  nFigure = IDM_ARC ;
     HDC          hdc ;
     HMENU        hMenu ; 
     HPEN         hPen ;
     PAINTSTRUCT  ps ;

     switch (message)
          {
          case WM_SIZE:
               x3 = y3 = 0 ;
               x4 = cxClient = LOWORD (lParam) ;
               y4 = cyClient = HIWORD (lParam) ;
               x2 = 3 * (x1 = cxClient / 4) ;
               y2 = 3 * (y1 = cyClient / 4) ;
               return 0 ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDM_ARC:
                    case IDM_CHORD:
                    case IDM_PIE:
                         hMenu = GetMenu (hwnd) ;
                         CheckMenuItem (hMenu, nFigure, MF_UNCHECKED) ;
                         CheckMenuItem (hMenu, nFigure = wParam, MF_CHECKED) ;
                         InvalidateRect (hwnd, NULL, FALSE) ;
                         return 0 ;
                    }
               break ;

          case WM_LBUTTONDOWN:
               if (!(wParam & MK_SHIFT))
                    {
                    x3 = LOWORD (lParam) ;
                    y3 = HIWORD (lParam) ;
                    InvalidateRect (hwnd, NULL, TRUE) ;
                    return 0 ;
                    }    
                                        // fall through for MK_SHIFT
          case WM_RBUTTONDOWN:
               x4 = LOWORD (lParam) ;   
               y4 = HIWORD (lParam) ;
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               hPen = SelectObject (hdc, CreatePen (PS_DOT, 1, 0L)) ;
               Rectangle (hdc, x1, y1, x2, y2) ;
               Ellipse   (hdc, x1, y1, x2, y2) ;

               DeleteObject (SelectObject (hdc, CreatePen (PS_SOLID, 3, 0L))) ;

               switch (nFigure)
                    {
                    case IDM_ARC:
                         Arc (hdc, x1, y1, x2, y2, x3, y3, x4, y4) ;
                         break ;

                    case IDM_CHORD:
                         Chord (hdc, x1, y1, x2, y2, x3, y3, x4, y4) ;
                         break ;

                    case IDM_PIE:
                         Pie (hdc, x1, y1, x2, y2, x3, y3, x4, y4) ;
                         break ;
                    }

               DeleteObject (SelectObject (hdc, hPen)) ;

               MoveTo (hdc, x3, y3) ;
               LineTo (hdc, cxClient / 2, cyClient / 2) ;
               LineTo (hdc, x4, y4) ;

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
