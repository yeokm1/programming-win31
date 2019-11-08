/*----------------------------------------------
   POPPRNT.C -- Popup Editor Printing Functions
  ----------------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include "poppad.h"

BOOL bUserAbort ;
HWND hDlgPrint ;

BOOL FAR PASCAL _export PrintDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
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

BOOL FAR PASCAL _export AbortProc (HDC hPrinterDC, short nCode)
     {
     MSG msg ;

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

BOOL PopPrntPrintFile (HANDLE hInst, HWND hwnd, HWND hwndEdit,
                                     LPSTR szTitleName)
     {
     static PRINTDLG pd ;
     BOOL            bSuccess ;
     char            szJobName [40] ;
     FARPROC         lpfnAbortProc, lpfnPrintDlgProc ;
     NPSTR           npstrBuffer ;
     short           yChar, nCharsPerLine, nLinesPerPage, nTotalLines,
                     nTotalPages, nPage, nLine, nLineNum ;
     TEXTMETRIC      tm ;
     WORD            nColCopy, nNonColCopy ;

     pd.lStructSize         = sizeof (PRINTDLG) ;
     pd.hwndOwner           = hwnd ;
     pd.hDevMode            = NULL ;
     pd.hDevNames           = NULL ;
     pd.hDC                 = NULL ;
     pd.Flags               = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC ;
     pd.nFromPage           = 0 ;
     pd.nToPage             = 0 ;
     pd.nMinPage            = 0 ;
     pd.nMaxPage            = 0 ;
     pd.nCopies             = 1 ;
     pd.hInstance           = NULL ;
     pd.lCustData           = 0L ;
     pd.lpfnPrintHook       = NULL ;
     pd.lpfnSetupHook       = NULL ;
     pd.lpPrintTemplateName = NULL ;
     pd.lpSetupTemplateName = NULL ;
     pd.hPrintTemplate      = NULL ;
     pd.hSetupTemplate      = NULL ;

     if (!PrintDlg (&pd))
          return TRUE ;

     nTotalLines = (short) SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0L) ;

     if (nTotalLines == 0)
          return TRUE ;

     GetTextMetrics (pd.hDC, &tm) ;
     yChar = tm.tmHeight + tm.tmExternalLeading ;

     nCharsPerLine = GetDeviceCaps (pd.hDC, HORZRES) / tm.tmAveCharWidth ;
     nLinesPerPage = GetDeviceCaps (pd.hDC, VERTRES) / yChar ;
     nTotalPages   = (nTotalLines + nLinesPerPage - 1) / nLinesPerPage ;

     npstrBuffer = (NPSTR) LocalAlloc (LPTR, nCharsPerLine + 1) ;

     EnableWindow (hwnd, FALSE) ;

     bSuccess   = TRUE ;
     bUserAbort = FALSE ;

     lpfnPrintDlgProc = MakeProcInstance ((FARPROC) PrintDlgProc, hInst) ;
     hDlgPrint = CreateDialog (hInst, "PrintDlgBox", hwnd, lpfnPrintDlgProc) ;
     SetDlgItemText (hDlgPrint, IDD_FNAME, szTitleName) ;

     lpfnAbortProc = MakeProcInstance ((FARPROC) AbortProc, hInst) ;
     Escape (pd.hDC, SETABORTPROC, 0, (LPSTR) lpfnAbortProc, NULL) ;

     GetWindowText (hwnd, szJobName, sizeof (szJobName)) ;

     if (Escape (pd.hDC, STARTDOC, strlen (szJobName), szJobName, NULL) > 0)
          {
          for (nColCopy = 0 ;
               nColCopy < (pd.Flags & PD_COLLATE ? pd.nCopies : 1) ;
               nColCopy++)
               {
               for (nPage = 0 ; nPage < nTotalPages ; nPage++)
                    {
                    for (nNonColCopy = 0 ;
                         nNonColCopy < (pd.Flags & PD_COLLATE ? 1 : pd.nCopies);
                         nNonColCopy++)
                         {
                         for (nLine = 0 ; nLine < nLinesPerPage ; nLine++)
                              {
                              nLineNum = nLinesPerPage * nPage + nLine ;

                              if (nLineNum > nTotalLines)
                                   break ;

                              * (short *) npstrBuffer = nCharsPerLine ;

                              TextOut (pd.hDC, 0, yChar * nLine, npstrBuffer,
                                   (short) SendMessage (hwndEdit, EM_GETLINE,
                                   nLineNum, (LONG) (LPSTR) npstrBuffer)) ;
                              }

                         if (Escape (pd.hDC, NEWFRAME, 0, NULL, NULL) < 0)
                              {
                              bSuccess = FALSE ;
                              break ;
                              }

                         if (bUserAbort)
                              break ;
                         }

                    if (!bSuccess || bUserAbort)
                         break ;
                    }

               if (!bSuccess || bUserAbort)
                    break ;
               }
          }
     else
          bSuccess = FALSE ;

     if (bSuccess)
          Escape (pd.hDC, ENDDOC, 0, NULL, NULL) ;

     if (!bUserAbort)
          {
          EnableWindow (hwnd, TRUE) ;
          DestroyWindow (hDlgPrint) ;
          }

     LocalFree ((LOCALHANDLE) npstrBuffer) ;
     FreeProcInstance (lpfnPrintDlgProc) ;
     FreeProcInstance (lpfnAbortProc) ;
     DeleteDC (pd.hDC) ;

     return bSuccess && !bUserAbort ;
     }
