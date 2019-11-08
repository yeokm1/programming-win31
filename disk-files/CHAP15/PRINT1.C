/*---------------------------------------
   PRINT1.C -- Bare Bones Printing
               (c) Charles Petzold, 1992
  ---------------------------------------*/

#include <windows.h>

HDC  GetPrinterDC (void) ;              // in PRINT.C
void PageGDICalls (HDC, short, short) ;

HANDLE hInst ;
char   szAppName [] = "Print1" ;
char   szCaption [] = "Print Program 1" ;

BOOL PrintMyPage (HWND hwnd)
     {
     static char szMessage [] = "Print1: Printing" ;
     BOOL        bError = FALSE ;
     HDC         hdcPrn ;
     short       xPage, yPage ;

     if (NULL == (hdcPrn = GetPrinterDC ()))
          return TRUE ;

     xPage = GetDeviceCaps (hdcPrn, HORZRES) ;
     yPage = GetDeviceCaps (hdcPrn, VERTRES) ;

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

     DeleteDC (hdcPrn) ;
     return bError ;
     }
