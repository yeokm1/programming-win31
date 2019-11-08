/*-----------------------------------------------
   DDEPOP2.C -- DDEML Server for Population Data
                (c) Charles Petzold, 1992
  -----------------------------------------------*/

#include <windows.h>
#include <ddeml.h>
#include <string.h>
#include "ddepop.h"

#define WM_USER_INITIATE (WM_USER + 1)
#define ID_TIMER         1

long     FAR PASCAL _export WndProc     (HWND, UINT, UINT, LONG) ;
HDDEDATA FAR PASCAL _export DdeCallback (UINT, UINT, HCONV, HSZ, HSZ,
                                         HDDEDATA, DWORD, DWORD) ;

char   szAppName [] = "DdePop2" ;
char   szTopic   [] = "US_Population" ;
DWORD  idInst ;
HANDLE hInst ;
HWND   hwnd ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     FARPROC  pfnDdeCallback ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (hPrevInstance) 
          return FALSE ;

     wndclass.style         = 0 ;
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

     hwnd = CreateWindow (szAppName, "DDEML Population Server",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, SW_SHOWMINNOACTIVE) ;
     UpdateWindow (hwnd) ;

               // Initialize for using DDEML

     pfnDdeCallback = MakeProcInstance ((FARPROC) DdeCallback, hInstance) ;

     if (DdeInitialize (&idInst, (PFNCALLBACK) pfnDdeCallback,
                        CBF_FAIL_EXECUTES | CBF_FAIL_POKES |
                        CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS, 0L))
          {
          MessageBox (hwnd, "Could not initialize server!",
                      szAppName, MB_ICONEXCLAMATION | MB_OK) ;

          DestroyWindow (hwnd) ;
          return FALSE ;
          }

                // Set the timer

     if (!SetTimer (hwnd, ID_TIMER, 5000, NULL))
          {
          MessageBox (hwnd, "Too many clocks or timers!", szAppName,
                      MB_ICONEXCLAMATION | MB_OK) ;

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

               // Clean up

     DdeUninitialize (idInst) ;
     KillTimer (hwnd, ID_TIMER) ;

     return msg.wParam ;
     }

int GetStateNumber (UINT iFmt, HSZ hszItem)
     {
     char szItem [32] ;
     int  i ;

     if (iFmt != CF_TEXT)
          return -1 ;

     DdeQueryString (idInst, hszItem, szItem, sizeof (szItem), 0) ;

     for (i = 0 ; i < NUM_STATES ; i++)
          if (strcmp (szItem, pop[i].szState) == 0)
               break ;

     if (i >= NUM_STATES)
          return -1 ;

     return i ;
     }

HDDEDATA FAR PASCAL _export DdeCallback (UINT iType, UINT iFmt, HCONV hConv,
                                         HSZ hsz1, HSZ hsz2, HDDEDATA hData,
                                         DWORD dwData1, DWORD dwData2)
     {
     char szBuffer [32] ;
     int  i ;

     switch (iType)
          {
          case XTYP_CONNECT:            // hsz1 = topic
                                        // hsz2 = service

               DdeQueryString (idInst, hsz2, szBuffer, sizeof (szBuffer), 0) ;

               if (0 != strcmp (szBuffer, szAppName))
                    return FALSE ;

               DdeQueryString (idInst, hsz1, szBuffer, sizeof (szBuffer), 0) ;

               if (0 != strcmp (szBuffer, szTopic))
                    return FALSE ;

               return TRUE ;

          case XTYP_ADVSTART:           // hsz1 = topic
                                        // hsz2 = item

                    // Check for matching format and data item

               if (-1 == (i = GetStateNumber (iFmt, hsz2)))
                    return FALSE ;

               pop[i].lPopLast = 0 ;
               PostMessage (hwnd, WM_TIMER, 0, 0L) ;

               return TRUE ;

          case XTYP_REQUEST:
          case XTYP_ADVREQ:             // hsz1 = topic
                                        // hsz2 = item

                    // Check for matching format and data item

               if (-1 == (i = GetStateNumber (iFmt, hsz2)))
                    return NULL ;

               wsprintf (szBuffer, "%ld\r\n", pop[i].lPop) ;

               return DdeCreateDataHandle (idInst, szBuffer,
                                           strlen (szBuffer) + 1,
                                           0, hsz2, CF_TEXT, 0) ;

          case XTYP_ADVSTOP:            // hsz1 = topic
                                        // hsz2 = item

                    // Check for matching format and data item

               if (-1 == (i = GetStateNumber (iFmt, hsz2)))
                    return FALSE ;

               return TRUE ;
          }

     return NULL ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static HSZ  hszService, hszTopic ;
     HSZ         hszItem ;
     int         i ;

     switch (message)
          {
          case WM_USER_INITIATE:
               InitPops () ;

               hszService = DdeCreateStringHandle (idInst, szAppName, 0) ;
               hszTopic   = DdeCreateStringHandle (idInst, szTopic,   0) ;

               DdeNameService (idInst, hszService, NULL, DNS_REGISTER) ;
               return 0 ;

          case WM_TIMER:
          case WM_TIMECHANGE:

                    // Calculate new current populations

               CalcPops () ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    if (pop[i].lPop != pop[i].lPopLast)
                         {
                         hszItem = DdeCreateStringHandle (idInst,
                                                          pop[i].szState, 0) ;

                         DdePostAdvise (idInst, hszTopic, hszItem) ;

                         DdeFreeStringHandle (idInst, hszItem) ;

                         pop[i].lPopLast = pop[i].lPop ;
                         }

               return 0 ;

          case WM_QUERYOPEN:
               return 0 ;

          case WM_DESTROY:
               DdeNameService (idInst, hszService, NULL, DNS_UNREGISTER) ;
               DdeFreeStringHandle (idInst, hszService) ;
               DdeFreeStringHandle (idInst, hszTopic) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
