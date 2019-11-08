/*---------------------------------------
   PRINT4.C -- Printing with Banding
               (c) Charles Petzold, 1992
  ---------------------------------------*/

#include <windows.h>

typedef BOOL (FAR PASCAL * ABORTPRC) (HDC, int) ;

HDC  GetPrinterDC (void) ;              // in PRINT.C

HANDLE hInst ;
char   szAppName [] = "Print4" ;
char   szCaption [] = "Print Program 4 (Banding)" ;

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

BOOL FAR PASCAL _export AbortProc (HDC hdcPrn, int nCode)
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
     static char szSpMsg [] = "Print4: Printing" ; 
     static char szText  [] = "Hello Printer!" ;
     ABORTPRC    lpfnAbortProc ;
     BOOL        bError = FALSE ;
     FARPROC     lpfnPrintDlgProc ;
     HDC         hdcPrn ;
     RECT        rect ;
     short       xPage, yPage ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
     yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

     EnableWindow (hwnd, FALSE) ;

     bUserAbort = FALSE ;
     lpfnPrintDlgProc = MakeProcInstance ((FARPROC) PrintDlgProc, hInst) ;
     hDlgPrint = CreateDialog (hInst, "PrintDlgBox", hwnd, lpfnPrintDlgProc) ;

     lpfnAbortProc = (ABORTPRC) MakeProcInstance ((FARPROC) AbortProc, hInst) ;
     Escape (hdcPrn, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;
                                        
     if (Escape (hdcPrn, STARTDOC, sizeof szSpMsg - 1, szSpMsg, NULL) > 0 &&
         Escape (hdcPrn, NEXTBAND, 0, NULL, (LPSTR) &rect) > 0)
          {
          while (!IsRectEmpty (&rect) && !bUserAbort)
               {
               lpfnAbortProc (hdcPrn, 0) ;

               Rectangle (hdcPrn, rect.left, rect.top, rect.right, 
                                                       rect.bottom) ;
               lpfnAbortProc (hdcPrn, 0) ;
          
               MoveTo (hdcPrn, 0, 0) ;
               LineTo (hdcPrn, xPage, yPage) ;

               lpfnAbortProc (hdcPrn, 0) ;

               MoveTo (hdcPrn, xPage, 0) ;
               LineTo (hdcPrn, 0, yPage) ;

               SaveDC (hdcPrn) ;

               SetMapMode (hdcPrn, MM_ISOTROPIC) ;
               SetWindowExt   (hdcPrn, 1000, 1000) ;
               SetViewportExt (hdcPrn, xPage / 2, -yPage / 2) ;
               SetViewportOrg (hdcPrn, xPage / 2,  yPage / 2) ;

               lpfnAbortProc (hdcPrn, 0) ;

               Ellipse (hdcPrn, -500, 500, 500, -500) ;

               lpfnAbortProc (hdcPrn, 0) ;

               SetTextAlign (hdcPrn, TA_BASELINE | TA_CENTER) ;

               TextOut (hdcPrn, 0, 0, szText, sizeof szText - 1) ;

               RestoreDC (hdcPrn, -1) ;

               lpfnAbortProc (hdcPrn, 0) ;

               if (Escape (hdcPrn, NEXTBAND, 0, NULL, (LPSTR) &rect) < 0)
                    {
                    bError = TRUE ;
                    break ;
                    }
               }
          }
     else
          bError = TRUE ;

     if (!bError)
          {
          if (bUserAbort)
               Escape (hdcPrn, ABORTDOC, 0, NULL, NULL) ;
          else
               Escape (hdcPrn, ENDDOC, 0, NULL, NULL) ;
          }

     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;     
          DestroyWindow (hDlgPrint) ;
          }

     FreeProcInstance (lpfnPrintDlgProc) ;
     FreeProcInstance ((FARPROC) lpfnAbortProc) ;
     DeleteDC (hdcPrn) ;

     return bError || bUserAbort ;
     }
