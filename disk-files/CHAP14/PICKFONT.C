/*-----------------------------------------
   PICKFONT.C -- Font Picker Program
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>
#include "pickfont.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export DlgProc (HWND, UINT, UINT, LONG) ;

char    szAppName [] = "PickFont" ;
DWORD   dwAspectMatch = 0L ;
HWND    hDlg ;
LOGFONT lf ;
short   nMapMode = IDD_TEXT ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = NULL ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Font Picker",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          if (hDlg == 0 || !IsDialogMessage (hDlg, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage  (&msg) ;
               }
          }
     return msg.wParam ;
     }

void MySetMapMode (HDC hdc)
     {
     if (nMapMode == IDD_LTWPS)
          {
          SetMapMode (hdc, MM_ANISOTROPIC) ;
          SetWindowExt (hdc, 1440, 1440) ;
          SetViewportExt (hdc, GetDeviceCaps (hdc, LOGPIXELSX),
                               GetDeviceCaps (hdc, LOGPIXELSY)) ;
          }
     else
          SetMapMode (hdc, MM_TEXT + nMapMode - IDD_TEXT) ;
     }

void ShowMetrics (HWND hDlg)
     {
     static TEXTMETRIC tm ;
     static struct 
          {
          short nDlgID ;
          int   *pData ;
          }
          shorts [] = 
          {
          TM_HEIGHT,     &tm.tmHeight,
          TM_ASCENT,     &tm.tmAscent,
          TM_DESCENT,    &tm.tmDescent,
          TM_INTLEAD,    &tm.tmInternalLeading,
          TM_EXTLEAD,    &tm.tmExternalLeading,
          TM_AVEWIDTH,   &tm.tmAveCharWidth,
          TM_MAXWIDTH,   &tm.tmMaxCharWidth,
          TM_WEIGHT,     &tm.tmWeight,
          TM_OVER,       &tm.tmOverhang,
          TM_DIGX,       &tm.tmDigitizedAspectX,
          TM_DIGY,       &tm.tmDigitizedAspectY
          } ;
     static char    *szFamily [] = { "Don't Care", "Roman",  "Swiss",
                                     "Modern",     "Script", "Decorative" } ;
     BOOL           bTrans ;
     char           *szCharSet, szFaceName [LF_FACESIZE] ;
     HDC            hdc ;
     HFONT          hFont ;
     short          i ;

     lf.lfHeight    = GetDlgItemInt (hDlg, IDD_HEIGHT, &bTrans, TRUE) ;
     lf.lfWidth     = GetDlgItemInt (hDlg, IDD_WIDTH,  &bTrans, FALSE) ;
     lf.lfWeight    = GetDlgItemInt (hDlg, IDD_WEIGHT, &bTrans, FALSE) ;

     lf.lfItalic    = (BYTE) (IsDlgButtonChecked (hDlg, IDD_ITALIC) ? 1 : 0) ;
     lf.lfUnderline = (BYTE) (IsDlgButtonChecked (hDlg, IDD_UNDER)  ? 1 : 0) ;
     lf.lfStrikeOut = (BYTE) (IsDlgButtonChecked (hDlg, IDD_STRIKE) ? 1 : 0) ;

     GetDlgItemText (hDlg, IDD_FACE, (char *) lf.lfFaceName, LF_FACESIZE) ;

     dwAspectMatch = IsDlgButtonChecked (hDlg, IDD_ASPECT) ? 1L : 0L ;

     hdc = GetDC (hDlg) ;
     MySetMapMode (hdc) ;
     SetMapperFlags (hdc, dwAspectMatch) ;

     hFont = SelectObject (hdc, CreateFontIndirect (&lf)) ;
     GetTextMetrics (hdc, &tm) ;
     GetTextFace (hdc, sizeof szFaceName, szFaceName) ;

     DeleteObject (SelectObject (hdc, hFont)) ;
     ReleaseDC (hDlg, hdc) ;

     for (i = 0 ; i < sizeof shorts / sizeof shorts [0] ; i++)
          SetDlgItemInt (hDlg, shorts[i].nDlgID, *shorts[i].pData, TRUE) ;

     SetDlgItemText (hDlg, TM_PITCH, tm.tmPitchAndFamily & 1 ?
                                                      "VARIABLE":"FIXED") ;

     SetDlgItemText (hDlg, TM_FAMILY, szFamily [tm.tmPitchAndFamily >> 4]) ;

     switch (tm.tmCharSet)
          {
          case ANSI_CHARSET:      szCharSet = "ANSI" ;     break ;
          case SYMBOL_CHARSET:    szCharSet = "Symbol" ;   break ;
          case SHIFTJIS_CHARSET:  szCharSet = "Shift" ;    break ;
          case OEM_CHARSET:       szCharSet = "OEM" ;      break ;
          default:                szCharSet = "?????" ;    break ;
          }

     SetDlgItemText (hDlg, TM_CHARSET, szCharSet) ;
     SetDlgItemText (hDlg, TF_NAME, szFaceName) ;
     }

BOOL FAR PASCAL _export DlgProc (HWND hDlg, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               CheckRadioButton (hDlg, IDD_TEXT,   IDD_LTWPS,  IDD_TEXT) ; 
               CheckRadioButton (hDlg, IDD_ANSI,   IDD_OEM,    IDD_ANSI) ;
               CheckRadioButton (hDlg, IDD_PDEF,   IDD_PVAR,   IDD_PDEF) ;
               CheckRadioButton (hDlg, IDD_DONT,   IDD_DEC,    IDD_DONT) ;

               lf.lfEscapement    = 0 ;
               lf.lfOrientation   = 0 ;
               lf.lfQuality       = DEFAULT_QUALITY ;
               lf.lfOutPrecision  = OUT_DEFAULT_PRECIS ;
               lf.lfClipPrecision = CLIP_DEFAULT_PRECIS ;

               ShowMetrics (hDlg) ;
                                        // fall through
          case WM_SETFOCUS:
               SetFocus (GetDlgItem (hDlg, IDD_HEIGHT)) ;
               return FALSE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDD_TEXT:
                    case IDD_LOMET:
                    case IDD_HIMET:
                    case IDD_LOENG:
                    case IDD_HIENG:
                    case IDD_TWIPS:
                    case IDD_LTWPS:
                         CheckRadioButton (hDlg, IDD_TEXT, IDD_LTWPS, wParam) ;
                         nMapMode = wParam ;
                         break ;

                    case IDD_ASPECT:
                    case IDD_ITALIC:
                    case IDD_UNDER:
                    case IDD_STRIKE:
                         CheckDlgButton (hDlg, wParam, 
                              IsDlgButtonChecked (hDlg, wParam) ? 0 : 1) ;
                         break ;

                    case IDD_ANSI:
                         lf.lfCharSet = ANSI_CHARSET ;
                         CheckRadioButton (hDlg, IDD_ANSI, IDD_OEM, wParam) ;
                         break ;

                    case IDD_SYMBOL:
                         lf.lfCharSet = SYMBOL_CHARSET ;
                         CheckRadioButton (hDlg, IDD_ANSI, IDD_OEM, wParam) ;
                         break ;

                    case IDD_SHIFT:
                         lf.lfCharSet = SHIFTJIS_CHARSET ;
                         CheckRadioButton (hDlg, IDD_ANSI, IDD_OEM, wParam) ;
                         break ;

                    case IDD_OEM:
                         lf.lfCharSet = OEM_CHARSET ;
                         CheckRadioButton (hDlg, IDD_ANSI, IDD_OEM, wParam) ;
                         break ;

                    case IDD_PDEF:
                    case IDD_PFIXED:
                    case IDD_PVAR:
                         CheckRadioButton (hDlg, IDD_PDEF, IDD_PVAR, wParam) ;
                         lf.lfPitchAndFamily &= 0xF0 ;
                         lf.lfPitchAndFamily |= (BYTE) (wParam - IDD_PDEF) ;
                         break ;

                    case IDD_DONT:
                    case IDD_ROMAN:
                    case IDD_SWISS:
                    case IDD_MODERN:
                    case IDD_SCRIPT:
                    case IDD_DEC:
                         CheckRadioButton (hDlg, IDD_DONT, IDD_DEC, wParam) ;
                         lf.lfPitchAndFamily &= 0x0F ;
                         lf.lfPitchAndFamily |= (BYTE) (wParam-IDD_DONT << 4) ;
                         break ;

                    case IDD_OK:
                         ShowMetrics (hDlg) ;
                         InvalidateRect (GetParent (hDlg), NULL, TRUE) ;
                         break ;
                    }
               break ;

          default:
               return FALSE ;
          }
     return TRUE ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static char  szText [] =
                      "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPqQqRrSsTtUuVvWwXxYyZz" ;
     HANDLE       hInstance ;
     HDC          hdc ;
     HFONT        hFont ;
     FARPROC      lpfnDlgProc ;
     PAINTSTRUCT  ps ;
     RECT         rect ;
     
     switch (message)
          {
          case WM_CREATE :
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;
               lpfnDlgProc = MakeProcInstance ((FARPROC) DlgProc, hInstance) ;
               hDlg = CreateDialog (hInstance, szAppName, hwnd, lpfnDlgProc) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hDlg) ;
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;
               MySetMapMode (hdc) ;
               SetMapperFlags (hdc, dwAspectMatch) ;
               GetClientRect (hDlg, &rect) ;
               rect.bottom += 1 ;
               DPtoLP (hdc, (LPPOINT) &rect, 2) ;

               hFont = SelectObject (hdc, CreateFontIndirect (&lf)) ;

               TextOut (hdc, rect.left, rect.bottom, szText, 52) ;

               DeleteObject (SelectObject (hdc, hFont)) ;
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
