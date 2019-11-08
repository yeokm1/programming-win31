/*------------------------------------------
   POPFONT.C -- Popup Editor Font Functions
  ------------------------------------------*/

#include <windows.h>
#include <commdlg.h>

static LOGFONT logfont ;
static HFONT   hFont ;

BOOL PopFontChooseFont (HWND hwnd)
     {
     CHOOSEFONT cf ;

     cf.lStructSize      = sizeof (CHOOSEFONT) ;
     cf.hwndOwner        = hwnd ;
     cf.hDC              = NULL ;
     cf.lpLogFont        = &logfont ;
     cf.iPointSize       = 0 ;
     cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
                                                  | CF_EFFECTS ;
     cf.rgbColors        = 0L ;
     cf.lCustData        = 0L ;
     cf.lpfnHook         = NULL ;
     cf.lpTemplateName   = NULL ;
     cf.hInstance        = NULL ;
     cf.lpszStyle        = NULL ;
     cf.nFontType        = 0 ;               // Returned from ChooseFont
     cf.nSizeMin         = 0 ;
     cf.nSizeMax         = 0 ;

     return ChooseFont (&cf) ;
     }

void PopFontInitialize (HWND hwndEdit)
     {
     GetObject (GetStockObject (SYSTEM_FONT), sizeof (LOGFONT),
                                              (LPSTR) &logfont) ;
     hFont = CreateFontIndirect (&logfont) ;
     SendMessage (hwndEdit, WM_SETFONT, hFont, 0L) ;
     }

void PopFontSetFont (HWND hwndEdit)
     {
     HFONT hFontNew ;

     hFontNew = CreateFontIndirect (&logfont) ;
     SendMessage (hwndEdit, WM_SETFONT, hFontNew, 0L) ;
     DeleteObject (hFont) ;
     hFont = hFontNew ;
     }

void PopFontDeinitialize (void)
     {
     DeleteObject (hFont) ;
     }
