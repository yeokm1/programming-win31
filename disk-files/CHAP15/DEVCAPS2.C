/*------------------------------------------------------------------
   DEVCAPS2.C -- Displays Device Capability Information (Version 2)
                 (c) Charles Petzold, 1992
  ------------------------------------------------------------------*/

#include <windows.h>
#include <string.h>
#include "devcaps2.h"

void DoBasicInfo (HDC, HDC, short, short) ;            // in DEVCAPS.C
void DoOtherInfo (HDC, HDC, short, short) ;
void DoBitCodedCaps (HDC, HDC, short, short, short) ;

typedef VOID (FAR PASCAL *DEVMODEPROC) (HWND, HANDLE, LPSTR, LPSTR) ;

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "DevCaps2" ;
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

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, NULL,
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

void DoEscSupport (HDC hdc, HDC hdcInfo, short cxChar, short cyChar)
     {
     static struct
          {
          char  *szEscCode ;
          short nEscCode ;
          } 
          esc [] =
          {
          "NEWFRAME",          NEWFRAME,
          "ABORTDOC",          ABORTDOC,
          "NEXTBAND",          NEXTBAND,
          "SETCOLORTABLE",     SETCOLORTABLE,
          "GETCOLORTABLE",     GETCOLORTABLE,
          "FLUSHOUTPUT",       FLUSHOUTPUT,
          "DRAFTMODE",         DRAFTMODE,
          "QUERYESCSUPPORT",   QUERYESCSUPPORT,
          "SETABORTPROC",      SETABORTPROC,
          "STARTDOC",          STARTDOC,
          "ENDDOC",            ENDDOC,
          "GETPHYSPAGESIZE",   GETPHYSPAGESIZE,
          "GETPRINTINGOFFSET", GETPRINTINGOFFSET,
          "GETSCALINGFACTOR",  GETSCALINGFACTOR } ;

     static char *szYesNo [] = { "Yes", "No" } ;
     char        szBuffer [32] ;
     POINT       pt ;
     short       n, nReturn ;

     TextOut (hdc, cxChar, cyChar, "Escape Support", 14) ;

     for (n = 0 ; n < sizeof esc / sizeof esc [0] ; n++)
          {
          nReturn = Escape (hdcInfo, QUERYESCSUPPORT, 1,
                                   (LPSTR) & esc[n].nEscCode, NULL) ;
          TextOut (hdc, 6 * cxChar, (n + 3) * cyChar, szBuffer,
               wsprintf (szBuffer, "%-24s %3s", (LPSTR) esc[n].szEscCode,
                         (LPSTR) szYesNo [nReturn > 0 ? 0 : 1])) ;

          if (nReturn > 0 && esc[n].nEscCode >= GETPHYSPAGESIZE
                          && esc[n].nEscCode <= GETSCALINGFACTOR)
               {
               Escape (hdcInfo, esc[n].nEscCode, 0, NULL, (LPSTR) &pt) ;
               TextOut (hdc, 36 * cxChar, (n + 3) * cyChar, szBuffer,
                        wsprintf (szBuffer, "(%u,%u)", pt.x, pt.y)) ;
               }
          }
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static char   szAllDevices [4096], szDevice [32], szDriver [16],
                   szDriverFile [16], szWindowText [64] ;
     static HANDLE hLibrary ;
     static short  n, cxChar, cyChar, nCurrentDevice = IDM_SCREEN,
                                      nCurrentInfo   = IDM_BASIC ;
     char          *szOutput, *szPtr ;
     DEVMODEPROC   lpfnDM ;
     HDC           hdc, hdcInfo ;
     HMENU         hMenu ;
     PAINTSTRUCT   ps ;
     TEXTMETRIC    tm ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;
               ReleaseDC (hwnd, hdc) ;

               lParam = NULL ;
                                                  // fall through
          case WM_WININICHANGE:
               if (lParam != NULL && lstrcmp ((LPSTR) lParam, "devices") != 0)
                    return 0 ;

               hMenu = GetSubMenu (GetMenu (hwnd), 0) ;

               while (GetMenuItemCount (hMenu) > 1)
                    DeleteMenu (hMenu, 1, MF_BYPOSITION) ;

               GetProfileString ("devices", NULL, "", szAllDevices,
                         sizeof szAllDevices) ;

               n = IDM_SCREEN + 1 ;
               szPtr = szAllDevices ;
               while (*szPtr)
                    {
                    AppendMenu (hMenu, n % 16 ? 0 : MF_MENUBARBREAK, n, szPtr);
                    n++ ;
                    szPtr += strlen (szPtr) + 1 ;
                    }
               AppendMenu (hMenu, MF_SEPARATOR, 0, NULL) ;
               AppendMenu (hMenu, 0, IDM_DEVMODE, "Device Mode") ;

               wParam = IDM_SCREEN ;
                                                  // fall through
          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               if (wParam < IDM_DEVMODE)          // IDM_SCREEN & Printers
                    {
                    CheckMenuItem (hMenu, nCurrentDevice, MF_UNCHECKED) ;
                    nCurrentDevice = wParam ;
                    CheckMenuItem (hMenu, nCurrentDevice, MF_CHECKED) ;
                    }
               else if (wParam == IDM_DEVMODE)
                    {
                    GetMenuString (hMenu, nCurrentDevice, szDevice,
                                   sizeof szDevice, MF_BYCOMMAND) ;

                    GetProfileString ("devices", szDevice, "",
                                      szDriver, sizeof szDriver) ;

                    szOutput = strtok (szDriver, ", ") ;
                    strcat (strcpy (szDriverFile, szDriver), ".DRV") ;

                    if (hLibrary >= 32)
                         FreeLibrary (hLibrary) ;

                    hLibrary = LoadLibrary (szDriverFile) ;
                    if (hLibrary >= 32)
                         {
                         lpfnDM = (DEVMODEPROC)
                              GetProcAddress (hLibrary, "DEVICEMODE") ;

                         lpfnDM (hwnd, hLibrary, szDevice, szOutput) ;
                         }
                    }
               else                               // info menu items
                    {
                    CheckMenuItem (hMenu, nCurrentInfo, MF_UNCHECKED) ;
                    nCurrentInfo = wParam ;
                    CheckMenuItem (hMenu, nCurrentInfo, MF_CHECKED) ;
                    }
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_INITMENUPOPUP:
               if (lParam == 0)
                    EnableMenuItem (GetMenu (hwnd), IDM_DEVMODE,
                         nCurrentDevice == IDM_SCREEN ?
                              MF_GRAYED : MF_ENABLED) ;
               return 0 ;

          case WM_PAINT:
               strcpy (szWindowText, "Device Capabilities: ") ;
          
               if (nCurrentDevice == IDM_SCREEN)
                    {
                    strcpy (szDriver, "DISPLAY") ;
                    strcat (szWindowText, szDriver) ;
                    hdcInfo = CreateIC (szDriver, NULL, NULL, NULL) ;
                    }
               else
                    {
                    hMenu = GetMenu (hwnd) ;

                    GetMenuString (hMenu, nCurrentDevice, szDevice,
                                   sizeof szDevice, MF_BYCOMMAND) ;

                    GetProfileString ("devices", szDevice, "", szDriver, 10) ;
                    szOutput = strtok (szDriver, ", ") ;
                    strcat (szWindowText, szDevice) ;
                    
                    hdcInfo = CreateIC (szDriver, szDevice, szOutput, NULL) ;
                    }
               SetWindowText (hwnd, szWindowText) ;

               hdc = BeginPaint (hwnd, &ps) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

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

                         case IDM_ESC:
                              DoEscSupport (hdc, hdcInfo, cxChar, cyChar) ;
                              break ;
                         }
                    DeleteDC (hdcInfo) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               if (hLibrary >= 32)
                    FreeLibrary (hLibrary) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
