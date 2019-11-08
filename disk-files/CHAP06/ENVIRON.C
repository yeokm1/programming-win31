/*----------------------------------------
   ENVIRON.C -- Environment List Box
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define  MAXENV  4096

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Environ" ;
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
          wndclass.hbrBackground = COLOR_WINDOW + 1 ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Environment List Box",
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
     static char szBuffer [MAXENV + 1] ;
     static HWND hwndList, hwndText ;
     HDC         hdc ;
     TEXTMETRIC  tm ;
     WORD        n ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               GetTextMetrics (hdc, &tm) ;
               ReleaseDC (hwnd, hdc) ;

               hwndList = CreateWindow ("listbox", NULL,
                              WS_CHILD | WS_VISIBLE | LBS_STANDARD,
                              tm.tmAveCharWidth, tm.tmHeight * 3,
                              tm.tmAveCharWidth * 16 +
                                   GetSystemMetrics (SM_CXVSCROLL),
                              tm.tmHeight * 5,
                              hwnd, 1,
                              GetWindowWord (hwnd, GWW_HINSTANCE), NULL) ;

               hwndText = CreateWindow ("static", NULL,
                              WS_CHILD | WS_VISIBLE | SS_LEFT,
                              tm.tmAveCharWidth,          tm.tmHeight,
                              tm.tmAveCharWidth * MAXENV, tm.tmHeight,
                              hwnd, 2,
                              GetWindowWord (hwnd, GWW_HINSTANCE), NULL) ;

               for (n = 0 ; environ[n] ; n++)
                    {
                    if (strlen (environ [n]) > MAXENV)
                         continue ;
                    *strchr (strcpy (szBuffer, environ [n]), '=') = '\0' ;
                    SendMessage (hwndList, LB_ADDSTRING, 0,
                                 (LONG) (LPSTR) szBuffer) ;
                    }
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hwndList) ;
               return 0 ;

          case WM_COMMAND:
               if (wParam == 1 && HIWORD (lParam) == LBN_SELCHANGE)
                    {
                    n = (WORD) SendMessage (hwndList, LB_GETCURSEL, 0, 0L) ;
                    n = (WORD) SendMessage (hwndList, LB_GETTEXT, n,
                                            (LONG) (LPSTR) szBuffer) ;

                    strcpy (szBuffer + n + 1, getenv (szBuffer)) ;
                    *(szBuffer + n) = '=' ;

                    SetWindowText (hwndText, szBuffer) ;
                    }
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
