/*------------------------------------------
   SHOWPOP2.C -- DDEML Client using DDEPOP2
                 (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>
#include <ddeml.h>
#include <stdlib.h>
#include <string.h>
#include "showpop.h"

#define WM_USER_INITIATE (WM_USER + 1)
#define DDE_TIMEOUT      3000

long     FAR PASCAL _export WndProc     (HWND, UINT, UINT,  LONG) ;
HDDEDATA FAR PASCAL _export DdeCallback (UINT, UINT, HCONV, HSZ, HSZ,
                                         HDDEDATA, DWORD, DWORD) ;

char  szAppName [] = "ShowPop2" ;
DWORD idInst ;
HCONV hConv ;
HWND  hwnd ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     MSG      msg ;
     FARPROC  pfnDdeCallback ;
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

     hwnd = CreateWindow (szAppName, "DDEML Client - US Population",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

               // Initialize for using DDEML

     pfnDdeCallback = MakeProcInstance ((FARPROC) DdeCallback, hInstance) ;

     if (DdeInitialize (&idInst, (PFNCALLBACK) pfnDdeCallback,
                        APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L))
          {
          MessageBox (hwnd, "Could not initialize client!",
                      szAppName, MB_ICONEXCLAMATION | MB_OK) ;

          DestroyWindow (hwnd) ;
          return FALSE ;
          }

               // Start things going

     SendMessage (hwnd, WM_USER_INITIATE, 0, 0L) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }

               // Uninitialize DDEML

     DdeUninitialize (idInst) ;

     return msg.wParam ;
     }

HDDEDATA FAR PASCAL _export DdeCallback (UINT iType, UINT iFmt, HCONV hConv,
                                         HSZ hsz1, HSZ hsz2, HDDEDATA hData,
                                         DWORD dwData1, DWORD dwData2)
     {
     char szItem [10], szPopulation [16] ;
     int  i ;

     switch (iType)
          {
          case XTYP_ADVDATA:       // hsz1  = topic
                                   // hsz2  = item
                                   // hData = data

                    // Check for matching format and data item

               if (iFmt != CF_TEXT)
                    return DDE_FNOTPROCESSED ;

               DdeQueryString (idInst, hsz2, szItem, sizeof (szItem), 0) ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    if (strcmp (szItem, pop[i].szAbb) == 0)
                         break ;

               if (i >= NUM_STATES)
                    return DDE_FNOTPROCESSED ;

                    // Store the data and invalidate the window

               DdeGetData (hData, szPopulation, sizeof (szPopulation), 0) ;

               pop[i].lPop = atol (szPopulation) ;

               InvalidateRect (hwnd, NULL, FALSE) ;

               return DDE_FACK ;

          case XTYP_DISCONNECT:
               hConv = NULL ;

               MessageBox (hwnd, "The server has disconnected.",
                           szAppName, MB_ICONASTERISK | MB_OK) ;

               return NULL ;
          }

     return NULL ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static char  szService [] = "DdePop2",
                  szTopic   [] = "US_Population" ;
     static short cxChar, cyChar ;
     char         szBuffer [24] ;
     HDC          hdc ;
     HSZ          hszService, hszTopic, hszItem ;
     PAINTSTRUCT  ps ;
     short        i, x, y ;
     TEXTMETRIC   tm ;

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

                         // Try connecting

               hszService = DdeCreateStringHandle (idInst, szService, 0) ;
               hszTopic   = DdeCreateStringHandle (idInst, szTopic,   0) ;

               hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;

                         // If that doesn't work, load server

               if (hConv == NULL)
                    {
                    WinExec (szService, SW_SHOWMINNOACTIVE) ;

                    hConv = DdeConnect (idInst, hszService, hszTopic, NULL) ;
                    }

                         // Free the string handles

               DdeFreeStringHandle (idInst, hszService) ;
               DdeFreeStringHandle (idInst, hszTopic) ;

                         // If still not connected, display message box

               if (hConv == NULL)
                    {
                    MessageBox (hwnd, "Cannot connect with DDEPOP2.EXE!",
                                szAppName, MB_ICONEXCLAMATION | MB_OK) ;

                    return 0 ;
                    }

                         // Request notification

               for (i = 0 ; i < NUM_STATES ; i++)
                    {
                    hszItem = DdeCreateStringHandle (idInst, pop[i].szAbb, 0) ;

                    DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT,
                                          XTYP_ADVSTART | XTYPF_ACKREQ,
                                          DDE_TIMEOUT, NULL) ;

                    DdeFreeStringHandle (idInst, hszItem) ;
                    }

               if (i < NUM_STATES)
                    {
                    MessageBox (hwnd, "Failure on WM_DDE_ADVISE!",
                                szAppName, MB_ICONEXCLAMATION | MB_OK) ;
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

          case WM_CLOSE:
               if (hConv == NULL)
                    break ;

                         // Stop the advises

               for (i = 0 ; i < NUM_STATES ; i++)
                    {
                    hszItem = DdeCreateStringHandle (idInst, pop[i].szAbb, 0) ;

                    DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT,
                                          XTYP_ADVSTOP, DDE_TIMEOUT, NULL) ;

                    DdeFreeStringHandle (idInst, hszItem) ;
                    }

                         // Disconnect the conversation

               DdeDisconnect (hConv) ;

               break ;             // for default processing

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
