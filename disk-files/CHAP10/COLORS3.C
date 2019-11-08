/*----------------------------------------------
   COLORS3.C -- Version using Common Dialog Box
                (c) Charles Petzold, 1992
  ----------------------------------------------*/

#include <windows.h>
#include <commdlg.h>

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static CHOOSECOLOR cc ;
     static DWORD       dwCustColors [16] ;

     cc.lStructSize    = sizeof (CHOOSECOLOR) ;
     cc.hwndOwner      = NULL ;
     cc.hInstance      = NULL ;
     cc.rgbResult      = RGB (0x80, 0x80, 0x80) ;
     cc.lpCustColors   = dwCustColors ;
     cc.Flags          = CC_RGBINIT | CC_FULLOPEN ;
     cc.lCustData      = 0L ;
     cc.lpfnHook       = NULL ;
     cc.lpTemplateName = NULL ;

     return ChooseColor (&cc) ;
     }
