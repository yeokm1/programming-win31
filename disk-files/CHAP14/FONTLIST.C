/*-----------------------------------------
   FONTLIST.C -- Font Enumeration Program
                 (c) Charles Petzold, 1992
  -----------------------------------------*/

#include <windows.h>
#include <string.h>
#include <memory.h>
#include "fontlist.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

typedef struct
     {
     GLOBALHANDLE hGMem ;
     short        nCount ;
     }
     ENUMER ;

typedef struct
     {
     short        nFontType ;
     LOGFONT      lf ;
     TEXTMETRIC   tm ;
     }
     FONT ;

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
int  FAR PASCAL _export EnumAllFaces (LPLOGFONT, LPTEXTMETRIC, short,
                                      ENUMER FAR *) ;
int  FAR PASCAL _export EnumAllFonts (LPLOGFONT, LPTEXTMETRIC, short,
                                      ENUMER FAR *) ;
char szAppName[] = "FontList" ;

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
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }
     hwnd = CreateWindow (szAppName, "Font Enumeration",
                          WS_OVERLAPPEDWINDOW | WS_VSCROLL,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

int FAR PASCAL _export EnumAllFaces (LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                     short nFontType, ENUMER FAR *enumer)
     {
     LPSTR lpFaces ;

     if (NULL == GlobalReAlloc (enumer->hGMem,
                         (DWORD) LF_FACESIZE * (1 + enumer->nCount),
                         GMEM_MOVEABLE))
          return 0 ;

     lpFaces = GlobalLock (enumer->hGMem) ;
     lstrcpy (lpFaces + enumer->nCount * LF_FACESIZE,
              (char FAR *) lplf->lfFaceName) ;

     GlobalUnlock (enumer->hGMem) ;
     enumer->nCount ++ ;
     return 1 ;
     }

int FAR PASCAL _export EnumAllFonts (LPLOGFONT lplf, LPTEXTMETRIC lptm,
                                     short nFontType, ENUMER FAR *enumer)
     {
     FONT FAR *font ;

     if (NULL == GlobalReAlloc (enumer->hGMem,
                         (DWORD) sizeof (FONT) * (1 + enumer->nCount),
                         GMEM_MOVEABLE))
          return 0 ;

     font = (FONT FAR *) GlobalLock (enumer->hGMem) + enumer->nCount ;
     font->nFontType = nFontType ;
     _fmemcpy (&font->lf, lplf, sizeof (LOGFONT)) ;
     _fmemcpy (&font->tm, lptm, sizeof (TEXTMETRIC)) ;

     GlobalUnlock (enumer->hGMem) ;
     enumer->nCount ++ ;
     return 1 ;
     }

void Display (HDC hdc, short cxChar, short cyChar, FONT FAR *lpfont)
     {
     static FONT f ;

     static char *szYN [] = { "No",           "Yes"  };
     static char *szOP [] = { "Default",      "String",  "Char",    "Stroke" };
     static char *szCP [] = { "Default",      "Char",    "Stroke",   "?????" };
     static char *szQU [] = { "Draft",        "Default", "Proof",    "?????" };
     static char *szP1 [] = { "Default",      "Fixed",   "Variable", "?????" };
     static char *szP2 [] = { "Fixed",        "Variable"  };
     static char *szFA [] = { "Don't Care",   "Roman",      "Swiss", "Modern",
                              "Script",       "Decorative", "?????", "?????" };
     static char *szVR [] = { "Scalable",     "Raster"    };
     static char *szGD [] = { "GDI",          "Device"    };
     static char *szSC [] = { "Non-TrueType", "TrueType"  };

     static struct 
          {
          short x ;
          short y ;
          char  *szFmt ;
          int   *pData ;
          }
          shorts [] = 
          {
           1,  1, "LOGFONT",            NULL, 
           1,  2, "-------",            NULL,
           1,  3, "Height:      %10d",  &f.lf.lfHeight,
           1,  4, "Width:       %10d",  &f.lf.lfWidth,
           1,  5, "Escapement:  %10d",  &f.lf.lfEscapement,
           1,  6, "Orientation: %10d",  &f.lf.lfOrientation,
           1,  7, "Weight:      %10d",  &f.lf.lfWeight,
          28,  1, "TEXTMETRIC",         NULL,
          28,  2, "----------",         NULL,
          28,  3, "Height:       %5d",  &f.tm.tmHeight,
          28,  4, "Ascent:       %5d",  &f.tm.tmAscent,
          28,  5, "Descent:      %5d",  &f.tm.tmDescent,
          28,  6, "Int. Leading: %5d",  &f.tm.tmInternalLeading,
          28,  7, "Ext. Leading: %5d",  &f.tm.tmExternalLeading,
          28,  8, "Ave. Width:   %5d",  &f.tm.tmAveCharWidth,
          28,  9, "Max. Width:   %5d",  &f.tm.tmMaxCharWidth,
          28, 10, "Weight:       %5d",  &f.tm.tmWeight,
          51, 10, "Overhang:     %10d", &f.tm.tmOverhang,
          51, 11, "Digitized X:  %10d", &f.tm.tmDigitizedAspectX,
          51, 12, "Digitized Y:  %10d", &f.tm.tmDigitizedAspectY,
          } ;

     static struct 
          {
          short x ;
          short y ;
          char  *szFmt ;
          BYTE  *pData ;
          }
          bytes [] = 
          {
          51,  3, "First Char:   %10d", &f.tm.tmFirstChar,
          51,  4, "Last Char:    %10d", &f.tm.tmLastChar,
          51,  5, "Default Char: %10d", &f.tm.tmDefaultChar,
          51,  6, "Break Char:   %10d", &f.tm.tmBreakChar,
          } ;

     static struct 
          {
          short x ;
          short y ;
          char  *szFmt ;
          BYTE  *pData ;
          char  **szArray ;
          short sAnd ;
          short sShift ;
          }
          strings [] = 
          {
           1,  8, "Italic:      %10s",  &f.lf.lfItalic,         szYN, 1,    0,
           1,  9, "Underline:   %10s",  &f.lf.lfUnderline,      szYN, 1,    0,
           1, 10, "Strike-Out:  %10s",  &f.lf.lfStrikeOut,      szYN, 1,    0,
           1, 11, "Char Set:    %10s",  &f.lf.lfCharSet,        NULL, 0,    0,
           1, 12, "Out  Prec:   %10s",  &f.lf.lfOutPrecision,   szOP, 3,    0,
           1, 13, "Clip Prec:   %10s",  &f.lf.lfClipPrecision,  szCP, 3,    0,
           1, 14, "Quality:     %10s",  &f.lf.lfQuality,        szQU, 3,    0,
           1, 15, "Pitch:       %10s",  &f.lf.lfPitchAndFamily, szP1, 3,    0,
           1, 16, "Family:      %10s",  &f.lf.lfPitchAndFamily, szFA, 0x70, 4,
          28, 11, "Italic:       %5s",  &f.tm.tmItalic,         szYN, 1,    0,
          28, 12, "Underline:    %5s",  &f.tm.tmUnderlined,     szYN, 1,    0,
          28, 13, "Strike-Out:   %5s",  &f.tm.tmStruckOut,      szYN, 1,    0,
          51,  7, "Pitch:        %10s", &f.tm.tmPitchAndFamily, szP2, 1,    0,
          51,  8, "Family:       %10s", &f.tm.tmPitchAndFamily, szFA, 0x70, 4,
          51,  9, "Char Set:     %10s", &f.tm.tmCharSet,        NULL, 0,    0,
          28, 15, "Font Type:  %s",     (BYTE *) &f.nFontType,  szVR, 1,    0,
          50, 15, "%s",                 (BYTE *) &f.nFontType,  szGD, 2,    1,
          58, 15, "%s",                 (BYTE *) &f.nFontType,  szSC, 4,    2
          } ;
    
     char *szCharSet, szBuffer [80] ;
     int  i ;

     _fmemcpy (&f, lpfont, sizeof (FONT)) ;

     for (i = 0 ; i < sizeof shorts / sizeof shorts [0] ; i++)
          TextOut (hdc, cxChar * shorts[i].x, cyChar * shorts[i].y, szBuffer,
                   wsprintf (szBuffer, shorts[i].szFmt,
                             *shorts[i].pData)) ;

     for (i = 0 ; i < sizeof bytes / sizeof bytes [0] ; i++)
          TextOut (hdc, cxChar * bytes[i].x, cyChar * bytes[i].y, szBuffer,
                   wsprintf (szBuffer, bytes[i].szFmt,
                             *bytes[i].pData)) ;

     for (i = 0 ; i < sizeof strings / sizeof strings [0] ; i++)
          {
          if (strings[i].szArray == NULL)       // Character Set
               {
               switch (*strings[i].pData)
                    {
                    case ANSI_CHARSET:      szCharSet = "ANSI" ;       break ;
                    case SYMBOL_CHARSET:    szCharSet = "Symbol" ;     break ;
                    case SHIFTJIS_CHARSET:  szCharSet = "Shift JIS" ;  break ;
                    case OEM_CHARSET:       szCharSet = "OEM" ;        break ;
                    default:                szCharSet = "?????" ;      break ;
                    }
               TextOut (hdc, cxChar * strings[i].x,
                              cyChar * strings[i].y, szBuffer,
                        wsprintf (szBuffer, strings[i].szFmt,
                                  (LPSTR) szCharSet)) ;
               }
          else
               TextOut (hdc, cxChar * strings[i].x,
                             cyChar * strings[i].y, szBuffer,
                   wsprintf (szBuffer, strings[i].szFmt,
                             (LPSTR) ((strings[i].szArray)
                                  [(*strings[i].pData & strings[i].sAnd) >>
                                        strings[i].sShift]))) ;
          }

     TextOut (hdc, cxChar, cyChar * 17, szBuffer,
              wsprintf (szBuffer, "Face Name:   %10s",
                        (LPSTR) f.lf.lfFaceName)) ;
     }

HDC GetPrinterIC ()
     {
     char szPrinter [64] ;
     char *szDevice, *szDriver, *szOutput ;

     GetProfileString ("windows", "device", "", szPrinter, 64) ;

     if (NULL != (szDevice = strtok (szPrinter, "," )) &&
         NULL != (szDriver = strtok (NULL,      ", ")) &&
         NULL != (szOutput = strtok (NULL,      ", ")))
          
               return CreateIC (szDriver, szDevice, szOutput, NULL) ;

     return NULL ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static BOOL    bHaveInfo = FALSE ;
     static ENUMER  enumer1, enumer2 ;
     static FARPROC lpfnEnumAllFaces, lpfnEnumAllFonts ;
     static short   cxChar, cyChar, nCurrent ;
     static WORD    wCurrentDC = IDM_SCREEN ;
     HANDLE         hInstance ;
     HDC            hdc ;
     HFONT          hFont ;
     HMENU          hMenu ;
     FONT FAR       *font ;
     LPSTR          lpFaces ;
     PAINTSTRUCT    ps ;
     short          i ;
     TEXTMETRIC     tm ;

     switch (message)
          {
          case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)-> hInstance ;
               lpfnEnumAllFaces = MakeProcInstance ((FARPROC) EnumAllFaces,
                                                    hInstance) ;
               lpfnEnumAllFonts = MakeProcInstance ((FARPROC) EnumAllFonts,
                                                    hInstance) ;

               hdc = GetDC (hwnd) ;
               SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

               GetTextMetrics (hdc, (LPTEXTMETRIC) &tm) ;
               cxChar = tm.tmAveCharWidth ;
               cyChar = tm.tmHeight + tm.tmExternalLeading ;

               ReleaseDC (hwnd, hdc) ;
               return 0 ;
                              
          case WM_COMMAND:
               if (wParam == IDM_EXIT)
                    {
                    SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                    return 0 ;
                    }
               else if (wParam == wCurrentDC)
                    return 0 ;

               hMenu = GetMenu (hwnd) ;
               CheckMenuItem (hMenu, wCurrentDC, MF_UNCHECKED) ;
               CheckMenuItem (hMenu, wCurrentDC = wParam, MF_CHECKED) ;

                                        // fall through
          case WM_DEVMODECHANGE:
          case WM_FONTCHANGE:
               bHaveInfo = FALSE ;               
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          case WM_PAINT:
               if (!bHaveInfo)
                    {
                    if (enumer2.hGMem)
                         GlobalFree (enumer2.hGMem) ;

                    enumer1.hGMem  = GlobalAlloc (GHND, 1L) ;
                    enumer1.nCount = 0 ;

                    enumer2.hGMem  = GlobalAlloc (GHND, 1L) ;
                    enumer2.nCount = 0 ;

                    if (NULL == enumer1.hGMem || NULL == enumer2.hGMem)
                         goto MEMORY_ERROR ;

                    if (wCurrentDC == IDM_SCREEN)
                         hdc = CreateIC ("DISPLAY", NULL, NULL, NULL) ;
                    else
                         hdc = GetPrinterIC () ;

                    if (hdc)
                         {
                         if (0 == EnumFonts (hdc, NULL, lpfnEnumAllFaces,
                                                  (LPSTR) &enumer1))
                              goto MEMORY_ERROR ;

                         lpFaces = GlobalLock (enumer1.hGMem) ;

                         for (i = 0 ; i < enumer1.nCount ; i++)
                              if (0 == EnumFonts (hdc,
                                             lpFaces + i * LF_FACESIZE,
                                             lpfnEnumAllFonts,
                                             (LPSTR) &enumer2))
                                   goto MEMORY_ERROR ;

                         GlobalUnlock (enumer1.hGMem) ;
                         enumer2.nCount-- ;
     
                         DeleteDC (hdc) ;
                         bHaveInfo = TRUE ;
                         }
                    GlobalFree (enumer1.hGMem) ;
                    SetScrollRange (hwnd, SB_VERT, 0, enumer2.nCount, FALSE) ;
                    SetScrollPos   (hwnd, SB_VERT, nCurrent = 0, TRUE) ;
                    }

               hdc = BeginPaint (hwnd, &ps) ;

               if (bHaveInfo)
                    {
                    SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

                    font = (FONT FAR *) GlobalLock (enumer2.hGMem) + nCurrent ;
                    Display (hdc, cxChar, cyChar, font) ;

                    hFont = SelectObject (hdc, CreateFontIndirect (&font->lf));

                    TextOut (hdc, 1 * cxChar, 19 * cyChar,
                        "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz",
                         52) ;

                    GlobalUnlock (enumer2.hGMem) ;
                    DeleteObject (SelectObject (hdc, hFont)) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_KEYDOWN:
               switch (wParam)
                    {
                    case VK_HOME:
                         SendMessage (hwnd, WM_VSCROLL, SB_TOP, 0L) ;
                         break ;
                    case VK_END:
                         SendMessage (hwnd, WM_VSCROLL, SB_BOTTOM, 0L) ;
                         break ;
                    case VK_LEFT:
                    case VK_UP:
                    case VK_PRIOR:
                         SendMessage (hwnd, WM_VSCROLL, SB_LINEUP, 0L) ;
                         break ;
                    case VK_RIGHT:
                    case VK_DOWN:
                    case VK_NEXT:
                         SendMessage (hwnd, WM_VSCROLL, SB_LINEDOWN, 0L) ;
                         break ;
                    default:
                         return 0 ;
                    }
               return 0 ;

          case WM_VSCROLL:
               switch (wParam)
                    {
                    case SB_TOP:
                         nCurrent = 0 ;
                         break ;
                    case SB_BOTTOM:
                         nCurrent = enumer2.nCount ;
                         break ;
                    case SB_LINEUP:
                    case SB_PAGEUP:
                         nCurrent -- ;
                         break ;
                    case SB_LINEDOWN:
                    case SB_PAGEDOWN:
                         nCurrent ++ ;
                         break ;
                    case SB_THUMBPOSITION:
                         nCurrent = LOWORD (lParam) ;
                         break ;
                    default:
                         return 0 ;
                    }
               nCurrent = min (max (0, nCurrent), enumer2.nCount) ;
               SetScrollPos (hwnd, SB_VERT, nCurrent, TRUE) ;
               InvalidateRect (hwnd, NULL, TRUE) ;
               return 0 ;

          MEMORY_ERROR:
               MessageBox (hwnd, "Cannot allocate memory, must end.",
                    szAppName, MB_OK | MB_ICONSTOP | MB_SYSTEMMODAL) ;

                                             // fall through
          case WM_CLOSE:
               DestroyWindow (hwnd) ;
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
