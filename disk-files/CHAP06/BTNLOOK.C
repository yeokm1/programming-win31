/*----------------------------------------
   BTNLOOK.C -- Button Look Program
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>

struct
     {
     long style ;
     char *text ;
     }
     button[] =
     {
     BS_PUSHBUTTON,      "PUSHBUTTON",
     BS_DEFPUSHBUTTON,   "DEFPUSHBUTTON",
     BS_CHECKBOX,        "CHECKBOX", 
     BS_AUTOCHECKBOX,    "AUTOCHECKBOX",
     BS_RADIOBUTTON,     "RADIOBUTTON",
     BS_3STATE,          "3STATE",
     BS_AUTO3STATE,      "AUTO3STATE",
     BS_GROUPBOX,        "GROUPBOX",
     BS_AUTORADIOBUTTON, "AUTORADIO",
     BS_OWNERDRAW,       "OWNERDRAW"
     } ;

#define NUM (sizeof button / sizeof button [0])

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "BtnLook" ;
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

     hwnd = CreateWindow (szAppName, "Button Look",
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
     static char  szTop []    = "message         wParam       lParam",
                  szUnd []    = "_______         ______       ______",
                  szFormat [] = "%-16s%6X%8X-%04X",
                  szBuffer [50] ;
     static HWND  hwndButton [NUM] ;
     static RECT  rect ;
     static int   cxChar, cyChar ;
     HDC          hdc ;
     PAINTSTRUCT  ps ;
     int          i ;
     TEXTMETRIC   tm ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;
               ReleaseDC (hwnd, hdc) ;

               for (i = 0 ; i < NUM ; i++)
                    hwndButton [i] = CreateWindow ("button", button[i].text,
                              WS_CHILD | WS_VISIBLE | button[i].style,
                              cxChar, cyChar * (1 + 2 * i),
                              20 * cxChar, 7 * cyChar / 4,
                              hwnd, i,
                              ((LPCREATESTRUCT) lParam) -> hInstance, NULL) ;
               return 0 ;

          case WM_SIZE:
               rect.left   = 24 * cxChar ;
               rect.top    =  2 * cyChar ;
               rect.right  = LOWORD (lParam) ;
               rect.bottom = HIWORD (lParam) ;
               return 0 ;

          case WM_PAINT:
               InvalidateRect (hwnd, &rect, TRUE) ;

               hdc = BeginPaint (hwnd, &ps) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
               SetBkMode (hdc, TRANSPARENT) ;

               TextOut (hdc, 24 * cxChar, cyChar, szTop, sizeof szTop - 1);
               TextOut (hdc, 24 * cxChar, cyChar, szUnd, sizeof szUnd - 1);

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_COMMAND:
          case WM_DRAWITEM:
               ScrollWindow (hwnd, 0, -cyChar, &rect, &rect) ;
               hdc = GetDC (hwnd) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

               TextOut (hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
                        szBuffer,
                    wsprintf (szBuffer, szFormat, (LPSTR)
                        (message == WM_COMMAND ? "WM_COMMAND" : "WM_DRAWITEM"),
                        wParam, HIWORD (lParam), LOWORD (lParam))) ;

               ReleaseDC (hwnd, hdc) ;
               ValidateRect (hwnd, NULL) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
