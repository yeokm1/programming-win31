/*------------------------------------------------------
   DEVCAPS1.C -- Displays Device Capability Information
                 (c) Charles Petzold, 1992
  ------------------------------------------------------*/

#include <windows.h>
#include <string.h>
#include "devcaps1.h"

void DoBasicInfo (HDC, HDC, short, short) ;       // in DEVCAPS.C
void DoOtherInfo (HDC, HDC, short, short) ;
void DoBitCodedCaps (HDC, HDC, short, short, short) ;

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "DevCaps" ;
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          !RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Device Capabilities",
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

HDC GetPrinterIC ()
     {
     char szPrinter [64] ;
     char *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", "", szPrinter, 64) ;

     if (NULL != (szDevice = strtok (szPrinter, "," )) &&
         NULL != (szDriver = strtok (NULL,      ", ")) &&
         NULL != (szOutput = strtok (NULL,      ", ")))
          
               return CreateIC (szDriver, szDevice, szOutput, NULL) ;

     return NULL ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static short cxChar, cyChar, nCurrentDevice = IDM_SCREEN,
                                  nCurrentInfo   = IDM_BASIC ;
     HDC          hdc, hdcInfo ;
     HMENU        hMenu ;
     PAINTSTRUCT  ps ;
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
               return 0 ;

          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_SCREEN:
                    case IDM_PRINTER:
                         CheckMenuItem (hMenu, nCurrentDevice, MF_UNCHECKED) ;
                         nCurrentDevice = wParam ;
                         CheckMenuItem (hMenu, nCurrentDevice, MF_CHECKED) ;
                         InvalidateRect (hwnd, NULL, TRUE) ;
                         return 0 ;

                    case IDM_BASIC:
                    case IDM_OTHER:
                    case IDM_CURVE:
                    case IDM_LINE:
                    case IDM_POLY:
                    case IDM_TEXT:
                         CheckMenuItem (hMenu, nCurrentInfo, MF_UNCHECKED) ;
                         nCurrentInfo = wParam ;
                         CheckMenuItem (hMenu, nCurrentInfo, MF_CHECKED) ;
                         InvalidateRect (hwnd, NULL, TRUE) ;
                         return 0 ;
                    }
               break ;

          case WM_DEVMODECHANGE:
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
          
               if (nCurrentDevice == IDM_SCREEN)
                    hdcInfo = CreateIC ("DISPLAY", NULL, NULL, NULL) ;
               else
                    hdcInfo = GetPrinterIC () ;

               if (hdcInfo)
                    {
                    switch (nCurrentInfo)
                         {
                         case IDM_BASIC:
                              DoBasicInfo (hdc, hdcInfo, cxChar, cyChar) ;
                              break ;

                         case IDM_OTHER:
                              DoOtherInfo (hdc, hdcInfo, cxChar, cyChar) ;
                              break ;

                         case IDM_CURVE:
                         case IDM_LINE:
                         case IDM_POLY:
                         case IDM_TEXT:
                              DoBitCodedCaps (hdc, hdcInfo, cxChar, cyChar,
                                              nCurrentInfo - IDM_CURVE) ;
                              break ;
                         }
                    DeleteDC (hdcInfo) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
