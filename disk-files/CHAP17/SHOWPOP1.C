/*-----------------------------------------
   SHOWPOP1.C -- DDE Client using DDEPOP1
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>
#include <dde.h>
#include <stdlib.h>
#include <string.h>
#include "showpop.h"

#define WM_USER_INITIATE (WM_USER + 1)
#define DDE_TIMEOUT      3000

long FAR PASCAL _export WndProc  (HWND, UINT, UINT, LONG) ;

char szAppName [] = "ShowPop1" ;

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
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "DDE Client - US Population",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     SendMessage (hwnd, WM_USER_INITIATE, 0, 0L) ;

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
     static BOOL   fDoingInitiate = TRUE ;
     static char   szServerApp [] = "DdePop1",
                   szTopic     [] = "US_Population" ;
     static HWND   hwndServer = NULL ;
     static short  cxChar, cyChar ;
     ATOM          aApp, aTop, aItem ;
     char          szBuffer [24], szPopulation [16], szItem [16] ;
     DDEACK        DdeAck ;
     DDEDATA FAR   *lpDdeData ;
     DDEADVISE FAR *lpDdeAdvise ;
     DWORD         dwTime ;
     GLOBALHANDLE  hDdeAdvise, hDdeData ;
     HDC           hdc ;
     MSG           msg ;
     PAINTSTRUCT   ps ;
     short         i, x, y ;
     TEXTMETRIC    tm ;
     WORD          wStatus ;

     switch (message)
          {
          case WM_CREATE:
               hdc = GetDC (hwnd) ;
               GetTextMetrics (hdc, &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;
               ReleaseDC (hwnd, hdc) ;
               return 0 ;

          case WM_USER_INITIATE:

                     // Broadcast WM_DDE_INITIATE message

               aApp = GlobalAddAtom (szServerApp) ;
               aTop = GlobalAddAtom (szTopic) ;

               SendMessage (0xFFFF, WM_DDE_INITIATE, hwnd,
                            MAKELONG (aApp, aTop)) ;

                     // If no response, try loading DDEPOP first

               if (hwndServer == NULL)
                    {
                    WinExec (szServerApp, SW_SHOWMINNOACTIVE) ;

                    SendMessage (0xFFFF, WM_DDE_INITIATE, hwnd,
                                 MAKELONG (aApp, aTop)) ;
                    }

                    // Delete the atoms

               GlobalDeleteAtom (aApp) ;
               GlobalDeleteAtom (aTop) ;
               fDoingInitiate = FALSE ;

                    // If still no response, display message box

               if (hwndServer == NULL)
                    {
                    MessageBox (hwnd, "Cannot connect with DDEPOP1.EXE!",
                                szAppName, MB_ICONEXCLAMATION | MB_OK) ;

                    return 0 ;
                    }

                    // Post WM_DDE_ADVISE messages

               for (i = 0 ; i < NUM_STATES ; i++)
                    {
                    hDdeAdvise = GlobalAlloc (GHND | GMEM_DDESHARE,
                                              sizeof (DDEADVISE)) ;

                    lpDdeAdvise = (DDEADVISE FAR *) GlobalLock (hDdeAdvise) ;

                    lpDdeAdvise->fAckReq   = TRUE ;
                    lpDdeAdvise->fDeferUpd = FALSE ;
                    lpDdeAdvise->cfFormat  = CF_TEXT ;

                    GlobalUnlock (hDdeAdvise) ;

                    aItem = GlobalAddAtom (pop[i].szAbb) ;

                    if (!PostMessage (hwndServer, WM_DDE_ADVISE, hwnd,
                                      MAKELONG (hDdeAdvise, aItem)))
                         {
                         GlobalFree (hDdeAdvise) ;
                         GlobalDeleteAtom (aItem) ;
                         break ;
                         }

                    DdeAck.fAck = FALSE ;

                    dwTime = GetCurrentTime () ;

                    while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
                         {
                         if (PeekMessage (&msg, hwnd, WM_DDE_ACK,
                                          WM_DDE_ACK, PM_REMOVE))
                              {
                              GlobalDeleteAtom (HIWORD (msg.lParam)) ;

                              wStatus = LOWORD (msg.lParam) ;
                              DdeAck = * (DDEACK *) & wStatus ;

                              if (DdeAck.fAck == FALSE)
                                   GlobalFree (hDdeAdvise) ;

                              break ;
                              }
                         }

                    if (DdeAck.fAck == FALSE)
                         break ;

                    while (PeekMessage (&msg, hwnd, WM_DDE_FIRST,
                                        WM_DDE_LAST, PM_REMOVE))
                         {
                         DispatchMessage (&msg) ;
                         }
                    }

               if (i < NUM_STATES)
                    {
                    MessageBox (hwnd, "Failure on WM_DDE_ADVISE!",
                                szAppName, MB_ICONEXCLAMATION | MB_OK) ;
                    }
               return 0 ;

          case WM_DDE_ACK:

                    // In response to WM_DDE_INITIATE, save server window

               if (fDoingInitiate)
                    {
                    hwndServer = wParam ;
                    GlobalDeleteAtom (LOWORD (lParam)) ;
                    GlobalDeleteAtom (HIWORD (lParam)) ;
                    }
               return 0 ;

          case WM_DDE_DATA:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- DDEDATA memory handle
                    // HIWORD (lParam) -- item atom

               hDdeData  = LOWORD (lParam) ;
               lpDdeData = (DDEDATA FAR *) GlobalLock (hDdeData) ;
               aItem     = HIWORD (lParam) ;

                    // Initialize DdeAck structure

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

                    // Check for matching format and data item

               if (lpDdeData->cfFormat == CF_TEXT)
                    {
                    GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                    for (i = 0 ; i < NUM_STATES ; i++)
                         if (strcmp (szItem, pop[i].szAbb) == 0)
                              break ;

                    if (i < NUM_STATES)
                         {
                         lstrcpy (szPopulation, (char FAR *) lpDdeData->Value) ;
                         pop[i].lPop = atol (szPopulation) ;
                         InvalidateRect (hwnd, NULL, FALSE) ;

                         DdeAck.fAck = TRUE ;
                         }
                    }

                    // Acknowledge if necessary

               if (lpDdeData->fAckReq == TRUE)
                    {
                    wStatus = * (WORD *) & DdeAck ;

                    if (!PostMessage (wParam, WM_DDE_ACK, hwnd,
                                      MAKELONG (wStatus, aItem)))
                         {
                         GlobalDeleteAtom (aItem) ;
                         GlobalUnlock (hDdeData) ;
                         GlobalFree (hDdeData) ;
                         return 0 ;
                         }
                    }
               else
                    {
                    GlobalDeleteAtom (aItem) ;
                    }

                    // Clean up

               if (lpDdeData->fRelease == TRUE || DdeAck.fAck == FALSE)
                    {
                    GlobalUnlock (hDdeData) ;
                    GlobalFree (hDdeData) ;
                    }
               else
                    {
                    GlobalUnlock (hDdeData) ;
                    }

               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    {
                    if (i < (NUM_STATES + 1) / 2)
                         {
                         x = cxChar ;
                         y = i * cyChar ;
                         }
                    else
                         {
                         x = 44 * cxChar ;
                         y = (i - (NUM_STATES + 1) / 2) * cyChar ;
                         }

                    TextOut (hdc, x, y, szBuffer,
                             wsprintf (szBuffer, "%-20s",
                                       (LPSTR) pop[i].szState)) ;

                    x += 36 * cxChar ;

                    SetTextAlign (hdc, TA_RIGHT | TA_TOP) ;

                    TextOut (hdc, x, y, szBuffer,
                             wsprintf (szBuffer, "%10ld", pop[i].lPop)) ;

                    SetTextAlign (hdc, TA_LEFT | TA_TOP) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DDE_TERMINATE:

                    // Respond with another WM_DDE_TERMINATE message

               PostMessage (hwndServer, WM_DDE_TERMINATE, hwnd, 0L) ;
               hwndServer = NULL ;
               return 0 ;

          case WM_CLOSE:
               if (hwndServer == NULL)
                    break ;

                    // Post WM_DDE_UNADVISE message

               PostMessage (hwndServer, WM_DDE_UNADVISE, hwnd,
                            MAKELONG (CF_TEXT, NULL)) ;

               dwTime = GetCurrentTime () ;

               while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
                    {
                    if (PeekMessage (&msg, hwnd, WM_DDE_ACK,
                                     WM_DDE_ACK, PM_REMOVE))
                         break ;
                    }

                    // Post WM_DDE_TERMINATE message

               PostMessage (hwndServer, WM_DDE_TERMINATE, hwnd, 0L) ;

               dwTime = GetCurrentTime () ;

               while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
                    {
                    if (PeekMessage (&msg, hwnd, WM_DDE_TERMINATE,
                                     WM_DDE_TERMINATE, PM_REMOVE))
                         break ;
                    }

               break ;             // for default processing

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
