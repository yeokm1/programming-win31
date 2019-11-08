/*-----------------------------------------------------
   POPPAD2.C -- Popup Editor Version 2 (includes menu)
                (c) Charles Petzold, 1992
  -----------------------------------------------------*/

#include <windows.h>
#include "poppad2.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG);

char szAppName [] = "PopPad2" ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HANDLE   hAccel ;
     HWND     hwnd ;
     MSG      msg;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, szAppName,
                          WS_OVERLAPPEDWINDOW,
                          GetSystemMetrics (SM_CXSCREEN) / 4,
                          GetSystemMetrics (SM_CYSCREEN) / 4,
                          GetSystemMetrics (SM_CXSCREEN) / 2,
                          GetSystemMetrics (SM_CYSCREEN) / 2,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd); 

     hAccel = LoadAccelerators (hInstance, szAppName) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          if (!TranslateAccelerator (hwnd, hAccel, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          }
     return msg.wParam ;
     }

AskConfirmation (HWND hwnd)
     {
     return MessageBox (hwnd, "Really want to close PopPad2?",
                        szAppName, MB_YESNO | MB_ICONQUESTION) ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static HWND hwndEdit ;
     LONG        lSelect ;
     WORD        wEnable ;

     switch (message)
          {
          case WM_CREATE:
               hwndEdit = CreateWindow ("edit", NULL,
                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         0, 0, 0, 0,
                         hwnd, 1, ((LPCREATESTRUCT) lParam)->hInstance, NULL) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hwndEdit) ;
               return 0 ;

          case WM_SIZE: 
               MoveWindow (hwndEdit, 0, 0, LOWORD (lParam),
                                           HIWORD (lParam), TRUE) ;
               return 0 ;

          case WM_INITMENUPOPUP:
               if (lParam == 1)
                    {
                    EnableMenuItem (wParam, IDM_UNDO,
                         SendMessage (hwndEdit, EM_CANUNDO, 0, 0L) ?
                              MF_ENABLED : MF_GRAYED) ;

                    EnableMenuItem (wParam, IDM_PASTE,
                         IsClipboardFormatAvailable (CF_TEXT) ?
                              MF_ENABLED : MF_GRAYED) ;

                    lSelect = SendMessage (hwndEdit, EM_GETSEL, 0, 0L) ;

                    if (HIWORD (lSelect) == LOWORD (lSelect))
                         wEnable = MF_GRAYED ;
                    else
                         wEnable = MF_ENABLED ;

                    EnableMenuItem (wParam, IDM_CUT,   wEnable) ;
                    EnableMenuItem (wParam, IDM_COPY,  wEnable) ;
                    EnableMenuItem (wParam, IDM_DEL,   wEnable) ;

                    return 0 ;
                    }
               break ;

          case WM_COMMAND :
               if (LOWORD (lParam))
                    {
                    if (wParam == 1 && HIWORD (lParam) == EN_ERRSPACE)
                         MessageBox (hwnd, "Edit control out of space.",
                                     szAppName, MB_OK | MB_ICONSTOP) ;

                    return 0 ;
                    }

               else switch (wParam)
                         {
                         case IDM_NEW:
                         case IDM_OPEN:
                         case IDM_SAVE:
                         case IDM_SAVEAS:
                         case IDM_PRINT:
                              MessageBeep (0) ;
                              return 0 ;

                         case IDM_EXIT:
                              SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                              return 0 ;

                         case IDM_UNDO:
                              SendMessage (hwndEdit, WM_UNDO, 0, 0L) ;
                              return 0 ;

                         case IDM_CUT:
                              SendMessage (hwndEdit, WM_CUT, 0, 0L) ;
                              return 0 ;

                         case IDM_COPY:
                              SendMessage (hwndEdit, WM_COPY, 0, 0L) ;
                              return 0 ;

                         case IDM_PASTE:
                              SendMessage (hwndEdit, WM_PASTE, 0, 0L) ;
                              return 0 ;

                         case IDM_DEL:
                              SendMessage (hwndEdit, WM_CLEAR, 0, 0L) ;
                              return 0 ;

                         case IDM_SELALL:
                              SendMessage (hwndEdit, EM_SETSEL, 0,
                                           MAKELONG (0, 32767)) ;
                              return 0 ;

                         case IDM_HELP:
                              MessageBox (hwnd, "Help not yet implemented!",
                                   szAppName, MB_OK | MB_ICONEXCLAMATION) ;
                              return 0 ;

                         case IDM_ABOUT:
                              MessageBox (hwnd,
                                   "POPPAD2 (c) Charles Petzold, 1992",
                                   szAppName, MB_OK | MB_ICONINFORMATION) ;
                              return 0 ;
                         }
               break ;

          case WM_CLOSE:
               if (IDYES == AskConfirmation (hwnd))
                    DestroyWindow (hwnd) ;
               return 0 ;

          case WM_QUERYENDSESSION:
               if (IDYES == AskConfirmation (hwnd))
                    return 1L ;
               else
                    return 0 ;

          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
