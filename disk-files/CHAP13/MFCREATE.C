/*-----------------------------------------
   MFCREATE.C -- Metafile Creation Program
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HBRUSH hBrush  = CreateSolidBrush (RGB (0, 0, 255)) ;
     HDC    hdcMeta = CreateMetaFile ("MYLOGO.WMF") ;

     Rectangle (hdcMeta, 0, 0, 100, 100) ;
     MoveTo (hdcMeta, 0, 0) ;
     LineTo (hdcMeta, 100, 100) ;
     MoveTo (hdcMeta, 0, 100) ;
     LineTo (hdcMeta, 100, 0) ;
     SelectObject (hdcMeta, hBrush) ;
     Ellipse (hdcMeta, 20, 20, 80, 80) ;

     DeleteMetaFile (CloseMetaFile (hdcMeta)) ;
     DeleteObject (hBrush) ;

     MessageBeep (0) ;

     return FALSE ;
     }
