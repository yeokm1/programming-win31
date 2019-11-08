/*-----------------------------------------
   MENUDEMO.C -- Menu Demonstration
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>
#include "menudemo.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

char szAppName [] = "MenuDemo" ;

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
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Menu Demonstration",
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
     static int  wColorID [5] = { WHITE_BRUSH,  LTGRAY_BRUSH, GRAY_BRUSH,
                                  DKGRAY_BRUSH, BLACK_BRUSH } ;
     static WORD wSelection = IDM_WHITE ;
     HMENU       hMenu ;

     switch (message)
          {
          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_NEW:
                    case IDM_OPEN:
                    case IDM_SAVE:
                    case IDM_SAVEAS:
                         MessageBeep (0) ;
                         return 0 ;

                    case IDM_EXIT:
                         SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                         return 0 ;

                    case IDM_UNDO:
                    case IDM_CUT:
                    case IDM_COPY:
                    case IDM_PASTE:
                    case IDM_DEL:
                         MessageBeep (0) ;
                         return 0 ;

                    case IDM_WHITE:          // Note: Logic below
                    case IDM_LTGRAY:         //   assumes that IDM_WHITE
                    case IDM_GRAY:           //   through IDM_BLACK are
                    case IDM_DKGRAY:         //   consecutive numbers in
                    case IDM_BLACK:          //   the order shown here.

                         CheckMenuItem (hMenu, wSelection, MF_UNCHECKED) ;
                         wSelection = wParam ;
                         CheckMenuItem (hMenu, wSelection, MF_CHECKED) ;
     
                         SetClassWord (hwnd, GCW_HBRBACKGROUND,
                              GetStockObject (wColorID [wParam - IDM_WHITE])) ;

                         InvalidateRect (hwnd, NULL, TRUE) ;
                         return 0 ;

                    case IDM_START:
                         if (SetTimer (hwnd, 1, 1000, NULL))
                              {
                              EnableMenuItem (hMenu, IDM_START, MF_GRAYED) ;
                              EnableMenuItem (hMenu, IDM_STOP,  MF_ENABLED) ;
                              }
                         return 0 ;

                    case IDM_STOP:
                         KillTimer (hwnd, 1) ;
                         EnableMenuItem (hMenu, IDM_START, MF_ENABLED) ;
                         EnableMenuItem (hMenu, IDM_STOP,  MF_GRAYED) ;
                         return 0 ;

                    case IDM_HELP:
                         MessageBox (hwnd, "Help not yet implemented!",
                                     szAppName, MB_ICONEXCLAMATION | MB_OK) ;
                         return 0 ;

                    case IDM_ABOUT:
                         MessageBox (hwnd, "Menu Demonstration Program.",
                                     szAppName, MB_ICONINFORMATION | MB_OK) ;
                         return 0 ;
                    }
               break ;

          case WM_TIMER:
               MessageBeep (0) ;
               return 0 ;

          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
