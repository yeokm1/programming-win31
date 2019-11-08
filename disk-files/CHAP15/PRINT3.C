/*---------------------------------------
   PRINT3.C -- Printing with Dialog Box
               (c) Charles Petzold, 1992
  ---------------------------------------*/

#include <windows.h>

HDC  GetPrinterDC (void) ;              // in PRINT.C
void PageGDICalls (HDC, short, short) ;

HANDLE hInst ;
char   szAppName [] = "Print3" ;
char   szCaption [] = "Print Program 3 (Dialog Box)" ;

BOOL   bUserAbort ;
HWND   hDlgPrint ;

BOOL FAR PASCAL _export PrintDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               SetWindowText (hDlg, szAppName) ;
               EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE,
                                                            MF_GRAYED) ;
               return TRUE ;

          case WM_COMMAND:
               bUserAbort = TRUE ;
               EnableWindow (GetParent (hDlg), TRUE) ;
               DestroyWindow (hDlg) ;
               hDlgPrint = 0 ;
               return TRUE ;
          }
     return FALSE ;
     }          

BOOL FAR PASCAL _export AbortProc (HDC hdcPrn, short nCode)
     {
     MSG   msg ;

     while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          {
          if (!hDlgPrint || !IsDialogMessage (hDlgPrint, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage (&msg) ;
               }
          }
     return !bUserAbort ;
     }

BOOL PrintMyPage (HWND hwnd)
     {
     static char szMessage [] = "Print3: Printing" ; 
     BOOL        bError = FALSE ;
     FARPROC     lpfnAbortProc, lpfnPrintDlgProc ;
     HDC         hdcPrn ;
     short       xPage, yPage ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
     yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

     EnableWindow (hwnd, FALSE) ;

     bUserAbort = FALSE ;
     lpfnPrintDlgProc = MakeProcInstance ((FARPROC) PrintDlgProc, hInst) ;
     hDlgPrint = CreateDialog (hInst, "PrintDlgBox", hwnd, lpfnPrintDlgProc) ;

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
                                        
     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;     
          DestroyWindow (hDlgPrint) ;
          }

     FreeProcInstance (lpfnPrintDlgProc) ;
     FreeProcInstance (lpfnAbortProc) ;
     DeleteDC (hdcPrn) ;

     return bError || bUserAbort ;
     }
