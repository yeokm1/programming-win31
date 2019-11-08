/*---------------------------------------------
   FORMFEED.C -- Advances printer to next page
                 (c) Charles Petzold, 1992
  ---------------------------------------------*/

#include <windows.h>
#include <string.h>

HDC  GetPrinterDC (void) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szMsg [] = "FormFeed" ; 
     HDC         hdcPrint ;

     hdcPrint = GetPrinterDC () ;

     if (hdcPrint != NULL)
          {
          if (Escape (hdcPrint, STARTDOC, sizeof szMsg - 1, szMsg, NULL) > 0)
               if (Escape (hdcPrint, NEWFRAME, 0, NULL, NULL) > 0)
                    Escape (hdcPrint, ENDDOC, 0, NULL, NULL) ;

          DeleteDC (hdcPrint) ;
          }
     return FALSE ;
     }

HDC GetPrinterDC (void)
     {
     static char szPrinter [80] ;
     char        *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", ",,,", szPrinter, 80) ;

     if (NULL != (szDevice = strtok (szPrinter, "," )) &&
         NULL != (szDriver = strtok (NULL,      ", ")) &&
         NULL != (szOutput = strtok (NULL,      ", ")))
          
               return CreateDC (szDriver, szDevice, szOutput, NULL) ;

     return 0 ;
     }
