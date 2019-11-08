/*---------------------------------------------
   DDEPOP1.C -- DDE Server for Population Data
               (c) Charles Petzold, 1992
  ---------------------------------------------*/

#include <windows.h>
#include <dde.h>
#include <string.h>
#include "ddepop.h"

typedef struct
     {
     unsigned int fAdvise:1 ;
     unsigned int fDeferUpd:1 ;
     unsigned int fAckReq:1 ;
     unsigned int dummy:13 ;
     long         lPopPrev ;
     }
     POPADVISE ;

#define ID_TIMER    1
#define DDE_TIMEOUT 3000

long FAR PASCAL _export WndProc         (HWND, UINT, UINT, LONG) ;
long FAR PASCAL _export ServerProc      (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export TimerEnumProc   (HWND, LONG) ;
BOOL FAR PASCAL _export CloseEnumProc   (HWND, LONG) ;
BOOL            PostDataMessage (HWND, HWND, int, BOOL, BOOL, BOOL) ;

char   szAppName []     = "DdePop1" ;
char   szServerClass [] = "DdePop1.Server" ;
HANDLE hInst ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (hPrevInstance) 
          return FALSE ;

     hInst = hInstance ;

               // Register window class

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

               // Register window class for DDE Server

     wndclass.style         = 0 ;
     wndclass.lpfnWndProc   = ServerProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 2 * sizeof (WORD) ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = NULL ;
     wndclass.hbrBackground = NULL ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szServerClass ;

     RegisterClass (&wndclass) ;

     hwnd = CreateWindow (szAppName, "DDE Population Server",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     InitPops () ;                      // initialize 'pop' structure

     if (!SetTimer (hwnd, ID_TIMER, 5000, NULL))
          {
          MessageBox (hwnd, "Too many clocks or timers!", szAppName,
                      MB_ICONEXCLAMATION | MB_OK) ;

          return FALSE ;
          }

     ShowWindow (hwnd, SW_SHOWMINNOACTIVE) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }

     KillTimer (hwnd, ID_TIMER) ;

     return msg.wParam ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static FARPROC lpTimerEnumProc, lpCloseEnumProc ;
     static char    szTopic [] = "US_Population" ;
     ATOM           aApp, aTop ;
     HWND           hwndClient, hwndServer ;

     switch (message)
          {
          case WM_CREATE:
               lpTimerEnumProc = MakeProcInstance ((FARPROC) TimerEnumProc,
                                                   hInst) ;
               lpCloseEnumProc = MakeProcInstance ((FARPROC) CloseEnumProc,
                                                   hInst) ;
               return 0 ;

          case WM_DDE_INITIATE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- application atom
                    // HIWORD (lParam) -- topic atom

               hwndClient = wParam ;

               aApp = GlobalAddAtom (szAppName) ;
               aTop = GlobalAddAtom (szTopic) ;

                    // Check for matching atoms, create window, and acknowledge

               if ((LOWORD (lParam) == NULL || LOWORD (lParam) == aApp) &&
                   (HIWORD (lParam) == NULL || HIWORD (lParam) == aTop))
                    {
                    hwndServer = CreateWindow (szServerClass, NULL,
                                               WS_CHILD, 0, 0, 0, 0,
                                               hwnd, NULL, hInst, NULL) ;

                    SetWindowWord (hwndServer, 0, hwndClient) ;
                    SendMessage (wParam, WM_DDE_ACK, hwndServer,
                                 MAKELONG (aApp, aTop)) ;
                    }

                    // Otherwise, delete the atoms just created

               else
                    {
                    GlobalDeleteAtom (aApp) ;
                    GlobalDeleteAtom (aTop) ;
                    }

               return 0 ;

          case WM_TIMER:
          case WM_TIMECHANGE:
                    // Calculate new current populations

               CalcPops () ;

                    // Notify all child windows

               EnumChildWindows (hwnd, lpTimerEnumProc, 0L) ;
               return 0 ;

          case WM_QUERYOPEN:
               return 0 ;

          case WM_CLOSE:

                    // Notify all child windows

               EnumChildWindows (hwnd, lpCloseEnumProc, 0L) ;

               break ;                  // for default processing

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

long FAR PASCAL _export ServerProc (HWND hwnd, UINT message, UINT wParam,
                                                             LONG lParam)
     {
     ATOM          aItem ;
     char          szItem [10] ;
     DDEACK        DdeAck ;
     DDEADVISE FAR *lpDdeAdvise ;
     DWORD         dwTime ;
     GLOBALHANDLE  hPopAdvise, hDdeAdvise, hCommands, hDdePoke ;
     int           i ;
     HWND          hwndClient ;
     MSG           msg ;
     POPADVISE FAR *lpPopAdvise ;
     WORD          cfFormat, wStatus ;

     switch (message)
          {
          case WM_CREATE:

                    // Allocate memory for POPADVISE structures

               hPopAdvise = GlobalAlloc (GHND, NUM_STATES * sizeof (POPADVISE));

               if (hPopAdvise == NULL)
                    DestroyWindow (hwnd) ;
               else
                    SetWindowWord (hwnd, 2, hPopAdvise) ;

               return 0 ;

          case WM_DDE_REQUEST:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- data format
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               cfFormat   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

                    // Check for matching format and data item

               if (cfFormat == CF_TEXT)
                    {
                    GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                    for (i = 0 ; i < NUM_STATES ; i++)
                         if (strcmp (szItem, pop[i].szState) == 0)
                              break ;

                    if (i < NUM_STATES)
                         {
                         GlobalDeleteAtom (aItem) ;
                         PostDataMessage (hwnd, hwndClient, i,
                                          FALSE, FALSE, TRUE) ;
                         return 0 ;
                         }
                    }

                    // Negative acknowledge if no match

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_ADVISE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- DDEADVISE memory handle
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               hDdeAdvise = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               lpDdeAdvise = (DDEADVISE FAR *) GlobalLock (hDdeAdvise) ;

                    // Check for matching format and data item

               if (lpDdeAdvise->cfFormat == CF_TEXT)
                    {
                    GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                    for (i = 0 ; i < NUM_STATES ; i++)
                         if (strcmp (szItem, pop[i].szState) == 0)
                              break ;

                         // Fill in the POPADVISE structure and acknowledge

                    if (i < NUM_STATES)
                         {
                         hPopAdvise = GetWindowWord (hwnd, 2) ;
                         lpPopAdvise = (POPADVISE FAR *)
                                              GlobalLock (hPopAdvise) ;

                         lpPopAdvise[i].fAdvise   = TRUE ;
                         lpPopAdvise[i].fDeferUpd = lpDdeAdvise->fDeferUpd ;
                         lpPopAdvise[i].fAckReq   = lpDdeAdvise->fAckReq ;
                         lpPopAdvise[i].lPopPrev  = pop[i].lPop ;

                         GlobalUnlock (hDdeAdvise) ;
                         GlobalFree (hDdeAdvise) ;

                         DdeAck.bAppReturnCode = 0 ;
                         DdeAck.reserved       = 0 ;
                         DdeAck.fBusy          = FALSE ;
                         DdeAck.fAck           = TRUE ;

                         wStatus = * (WORD *) & DdeAck ;

                         if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                           MAKELONG (wStatus, aItem)))
                              {
                              GlobalDeleteAtom (aItem) ;
                              }
                         else
                              {
                              PostDataMessage (hwnd, hwndClient, i,
                                               lpPopAdvise[i].fDeferUpd,
                                               lpPopAdvise[i].fAckReq,
                                               FALSE) ;
                              }

                         GlobalUnlock (hPopAdvise) ;
                         return 0 ;
                         }
                    }

                         // Otherwise post a negative WM_DDE_ACK

               GlobalUnlock (hDdeAdvise) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalFree (hDdeAdvise) ;
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_UNADVISE:

                    // wParam          -- sending window handle
                    // LOWORD (lParam) -- data format
                    // HIWORD (lParam) -- item atom

               hwndClient = wParam ;
               cfFormat   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = TRUE ;

               hPopAdvise  = GetWindowWord (hwnd, 2) ;
               lpPopAdvise = (POPADVISE FAR *) GlobalLock (hPopAdvise) ;

                    // Check for matching format and data item

               if (cfFormat == CF_TEXT || cfFormat == 0)
                    {
                    if (aItem == NULL)
                         for (i = 0 ; i < NUM_STATES ; i++)
                              lpPopAdvise[i].fAdvise = FALSE ;
                    else
                         {
                         GlobalGetAtomName (aItem, szItem, sizeof (szItem)) ;

                         for (i = 0 ; i < NUM_STATES ; i++)
                              if (strcmp (szItem, pop[i].szState) == 0)
                                   break ;

                         if (i < NUM_STATES)
                              lpPopAdvise[i].fAdvise = FALSE ;
                         else
                              DdeAck.fAck = FALSE ;
                         }
                    }
               else
                    DdeAck.fAck = FALSE ;

                    // Acknowledge either positively or negatively

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    if (aItem != NULL)
                         GlobalDeleteAtom (aItem) ;
                    }

               GlobalUnlock (hPopAdvise) ;
               return 0 ;

          case WM_DDE_EXECUTE:

                    // Post negative acknowledge

               hwndClient = wParam ;
               hCommands  = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, hCommands)))
                    {
                    GlobalFree (hCommands) ;
                    }
               return 0 ;

          case WM_DDE_POKE:

                    // Post negative acknowledge

               hwndClient = wParam ;
               hDdePoke   = LOWORD (lParam) ;
               aItem      = HIWORD (lParam) ;

               DdeAck.bAppReturnCode = 0 ;
               DdeAck.reserved       = 0 ;
               DdeAck.fBusy          = FALSE ;
               DdeAck.fAck           = FALSE ;

               wStatus = * (WORD *) & DdeAck ;

               if (!PostMessage (hwndClient, WM_DDE_ACK, hwnd,
                                 MAKELONG (wStatus, aItem)))
                    {
                    GlobalFree (hDdePoke) ;
                    GlobalDeleteAtom (aItem) ;
                    }

               return 0 ;

          case WM_DDE_TERMINATE:

                    // Respond with another WM_DDE_TERMINATE message

               hwndClient = wParam ;
               PostMessage (hwndClient, WM_DDE_TERMINATE, hwnd, 0L) ;
               DestroyWindow (hwnd) ;
               return 0 ;

          case WM_TIMER:

                    // Post WM_DDE_DATA messages for changed populations

               hwndClient  = GetWindowWord (hwnd, 0) ;
               hPopAdvise  = GetWindowWord (hwnd, 2) ;
               lpPopAdvise = (POPADVISE FAR *) GlobalLock (hPopAdvise) ;

               for (i = 0 ; i < NUM_STATES ; i++)
                    if (lpPopAdvise[i].fAdvise)
                         if (lpPopAdvise[i].lPopPrev != pop[i].lPop)
                              {
                              if (!PostDataMessage (hwnd, hwndClient, i,
                                                    lpPopAdvise[i].fDeferUpd,
                                                    lpPopAdvise[i].fAckReq,
                                                    FALSE))
                                   break ;

                              lpPopAdvise[i].lPopPrev = pop[i].lPop ;
                              }

               GlobalUnlock (hPopAdvise) ;
               return 0 ;

          case WM_CLOSE:

                    // Post a WM_DDE_TERMINATE message to the client

               hwndClient = GetWindowWord (hwnd, 0) ;
               PostMessage (hwndClient, WM_DDE_TERMINATE, hwnd, 0L) ;

               dwTime = GetCurrentTime () ;

               while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
                    if (PeekMessage (&msg, hwnd, WM_DDE_TERMINATE,
                                     WM_DDE_TERMINATE, PM_REMOVE))
                         break ;

               DestroyWindow (hwnd) ;
               return 0 ;

          case WM_DESTROY:
               hPopAdvise = GetWindowWord (hwnd, 2) ;
               GlobalFree (hPopAdvise) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

BOOL FAR PASCAL _export TimerEnumProc (HWND hwnd, LONG lParam)
     {
     SendMessage (hwnd, WM_TIMER, 0, 0L) ;

     return TRUE ;
     }

BOOL FAR PASCAL _export CloseEnumProc (HWND hwnd, LONG lParam)
     {
     SendMessage (hwnd, WM_CLOSE, 0, 0L) ;

     return TRUE ;
     }

BOOL PostDataMessage (HWND hwndServer, HWND hwndClient, int iState,
                      BOOL fDeferUpd, BOOL fAckReq, BOOL fResponse)
     {
     ATOM         aItem ;
     char         szPopulation [16] ;
     DDEACK       DdeAck ;
     DDEDATA FAR  *lpDdeData ;
     DWORD        dwTime ;
     GLOBALHANDLE hDdeData ;
     MSG          msg ;
     WORD         wStatus ;

     aItem = GlobalAddAtom (pop[iState].szState) ;

          // Allocate a DDEDATA structure if not defered update

     if (fDeferUpd)
          {
          hDdeData = NULL ;
          }
     else
          {
          wsprintf (szPopulation, "%ld\r\n", pop[iState].lPop) ;

          hDdeData = GlobalAlloc (GHND | GMEM_DDESHARE,
                                  sizeof (DDEDATA) + strlen (szPopulation)) ;

          lpDdeData = (DDEDATA FAR *) GlobalLock (hDdeData) ;

          lpDdeData->fResponse = fResponse ;
          lpDdeData->fRelease  = TRUE ;
          lpDdeData->fAckReq   = fAckReq ;
          lpDdeData->cfFormat  = CF_TEXT ;

          lstrcpy ((LPSTR) lpDdeData->Value, szPopulation) ;

          GlobalUnlock (hDdeData) ;
          }

          // Post the WM_DDE_DATA message

     if (!PostMessage (hwndClient, WM_DDE_DATA, hwndServer,
                       MAKELONG (hDdeData, aItem)))
          {
          if (hDdeData != NULL)
               GlobalFree (hDdeData) ;

          GlobalDeleteAtom (aItem) ;
          return FALSE ;
          }

          // Wait for the acknowledge message if it's requested

     if (fAckReq)
          {
          DdeAck.fAck = FALSE ;

          dwTime = GetCurrentTime () ;

          while (GetCurrentTime () - dwTime < DDE_TIMEOUT)
               {
               if (PeekMessage (&msg, hwndServer, WM_DDE_ACK, WM_DDE_ACK,
                                PM_REMOVE))
                    {
                    wStatus = LOWORD (msg.lParam) ;
                    DdeAck = * (DDEACK *) & wStatus ;
                    aItem  = HIWORD (msg.lParam) ;
                    GlobalDeleteAtom (aItem) ;
                    break ;
                    }
               }

          if (DdeAck.fAck == FALSE)
               {
               if (hDdeData != NULL)
                    GlobalFree (hDdeData) ;

               return FALSE ;
               }
          }

     return TRUE ;
     }
