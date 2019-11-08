/*------------------------------------------
   PRINT2.C -- Printing with Abort Function
               (c) Charles Petzold, 1992
  ------------------------------------------*/

#include <windows.h>

HDC  GetPrinterDC (void) ;              // in PRINT.C
void PageGDICalls (HDC, short, short) ;

HANDLE hInst ;
char   szAppName [] = "Print2" ;
char   szCaption [] = "Print Program 2 (Abort Function)" ;

BOOL FAR PASCAL _export AbortProc (HDC hdcPrn, short nCode)
     {
     MSG msg ;

     while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return TRUE ;
     }

BOOL PrintMyPage (HWND hwnd)
     {
     static char szMessage [] = "Print2: Printing" ; 
     BOOL        bError = FALSE ;
     FARPROC     lpfnAbortProc ;
     HDC         hdcPrn ;
     short       xPage, yPage ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
     yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

     EnableWindow (hwnd, FALSE) ;

     lpfnAbortProc = MakeProcInstance ((FARPROC) AbortProc, hInst) ;
     Escape (hdcPrn, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;

     if (Escape (hdcPrn, STARTDOC, sizeof szMessage - 1, szMessage, NULL) > 0)
          {
          PageGDICalls (hdcPrn, xPage, yPage) ;

          if (Escape (hdcPrn, NEWFRAME, 0, NULL, NULL) > 0)
               Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;
          else
               bError = TRUE ;
          }
     else
          bError = TRUE ;

     if (!bError)
          Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;

     FreeProcInstance (lpfnAbortProc) ;
     EnableWindow (hwnd, TRUE) ;
     DeleteDC (hdcPrn) ;
     return bError ;
     }
