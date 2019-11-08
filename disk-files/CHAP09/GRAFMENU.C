/*----------------------------------------------
   GRAFMENU.C -- Demonstrates Bitmap Menu Items
                 (c) Charles Petzold, 1992
  ----------------------------------------------*/

#include <windows.h>
#include <string.h>
#include "grafmenu.h"

long FAR PASCAL _export WndProc  (HWND, UINT, UINT, LONG) ;
HBITMAP StretchBitmap (HBITMAP) ;
HBITMAP GetBitmapFont (int) ;

char szAppName [] = "GrafMenu" ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HBITMAP  hBitmapHelp, hBitmapFile, hBitmapEdit,
              hBitmapFont, hBitmapPopFont [3] ;
     HMENU    hMenu, hMenuPopup ;
     HWND     hwnd ;
     int      i ;
     MSG      msg ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
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

     hMenu = CreateMenu () ;

     hMenuPopup = LoadMenu (hInstance, "MenuFile") ;
     hBitmapFile = StretchBitmap (LoadBitmap (hInstance, "BitmapFile")) ;
     AppendMenu (hMenu, MF_BITMAP | MF_POPUP, hMenuPopup,
                 (LPSTR) (LONG) hBitmapFile);

     hMenuPopup = LoadMenu (hInstance, "MenuEdit") ;
     hBitmapEdit = StretchBitmap (LoadBitmap (hInstance, "BitmapEdit")) ;
     AppendMenu (hMenu, MF_BITMAP | MF_POPUP, hMenuPopup,
                 (LPSTR) (LONG) hBitmapEdit);

     hMenuPopup = CreateMenu () ;

     for (i = 0 ; i < 3 ; i++)
          {
          hBitmapPopFont [i] = GetBitmapFont (i) ;
          AppendMenu (hMenuPopup, MF_BITMAP, IDM_COUR + i,
                      (LPSTR) (LONG) hBitmapPopFont [i]) ;
          }

     hBitmapFont = StretchBitmap (LoadBitmap (hInstance, "BitmapFont")) ;
     AppendMenu (hMenu, MF_BITMAP | MF_POPUP, hMenuPopup,
                 (LPSTR) (LONG) hBitmapFont);

     hwnd = CreateWindow (szAppName, "Bitmap Menu Demonstration",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, hMenu, hInstance, NULL) ;

     hMenu = GetSystemMenu (hwnd, FALSE);
     hBitmapHelp = StretchBitmap (LoadBitmap (hInstance, "BitmapHelp")) ;
     AppendMenu (hMenu, MF_SEPARATOR, NULL,     NULL) ;
     AppendMenu (hMenu, MF_BITMAP,    IDM_HELP, (LPSTR) (LONG) hBitmapHelp) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }

     DeleteObject (hBitmapHelp) ;
     DeleteObject (hBitmapEdit) ;
     DeleteObject (hBitmapFile) ;
     DeleteObject (hBitmapFont) ;

     for (i = 0 ; i < 3 ; i++)
          DeleteObject (hBitmapPopFont [i]) ;

     return msg.wParam ;
     }

HBITMAP StretchBitmap (HBITMAP hBitmap1)
     {
     BITMAP     bm1, bm2 ;
     HBITMAP    hBitmap2 ;
     HDC        hdc, hdcMem1, hdcMem2 ;
     TEXTMETRIC tm ;

     hdc = CreateIC ("DISPLAY", NULL, NULL, NULL) ;
     GetTextMetrics (hdc, &tm) ;
     hdcMem1 = CreateCompatibleDC (hdc) ;
     hdcMem2 = CreateCompatibleDC (hdc) ;
     DeleteDC (hdc) ;

     GetObject (hBitmap1, sizeof (BITMAP), (LPSTR) &bm1) ;

     bm2 = bm1 ;
     bm2.bmWidth      = (tm.tmAveCharWidth * bm2.bmWidth)  / 4 ;
     bm2.bmHeight     = (tm.tmHeight       * bm2.bmHeight) / 8 ;
     bm2.bmWidthBytes = ((bm2.bmWidth + 15) / 16) * 2 ;

     hBitmap2 = CreateBitmapIndirect (&bm2) ;

     SelectObject (hdcMem1, hBitmap1) ;
     SelectObject (hdcMem2, hBitmap2) ;

     StretchBlt (hdcMem2, 0, 0, bm2.bmWidth, bm2.bmHeight,
                 hdcMem1, 0, 0, bm1.bmWidth, bm1.bmHeight, SRCCOPY) ;

     DeleteDC (hdcMem1) ;
     DeleteDC (hdcMem2) ;
     DeleteObject (hBitmap1) ;

     return hBitmap2 ;
     }

HBITMAP GetBitmapFont (int i)
     {
     static char    *szFaceName[3] = { "Courier New", "Arial",
                                       "Times New Roman" } ;
     static LOGFONT lf ;
     DWORD          dwSize ;
     HBITMAP        hBitmap ;
     HDC            hdc, hdcMem ;
     HFONT          hFont ;
     TEXTMETRIC     tm ;
     RECT           rc ;

     hdc = CreateIC ("DISPLAY", NULL, NULL, NULL) ;
     GetTextMetrics (hdc, &tm) ;

     lf.lfHeight = 2 * tm.tmHeight ;
     strcpy ((char *) lf.lfFaceName, szFaceName[i]) ;

     hdcMem = CreateCompatibleDC (hdc) ;
     SelectObject (hdcMem, CreateFontIndirect (&lf)) ;
     dwSize = GetTextExtent (hdcMem, szFaceName[i], strlen (szFaceName[i])) ;

     hBitmap = CreateBitmap (LOWORD (dwSize), HIWORD (dwSize), 1, 1, NULL) ;
     SelectObject (hdcMem, hBitmap) ;

     rc.left   = 0 ;
     rc.top    = 0 ;
     rc.right  = LOWORD (dwSize) ;
     rc.bottom = HIWORD (dwSize) ;

     FillRect (hdcMem, &rc, GetStockObject (WHITE_BRUSH)) ;
     TextOut (hdcMem, 0, 0, szFaceName[i], strlen (szFaceName[i])) ;

     DeleteDC (hdcMem) ;
     DeleteDC (hdc) ;
     DeleteObject (hFont) ;

     return hBitmap ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     HMENU  hMenu ;
     static short nCurrentFont = IDM_COUR ;

     switch (message)
          {
          case WM_CREATE:
               CheckMenuItem (GetMenu (hwnd), nCurrentFont, MF_CHECKED) ;
               return 0 ;

          case WM_SYSCOMMAND:
               switch (wParam)
                    {
                    case IDM_HELP:
                         MessageBox (hwnd, "Help not yet implemented.",
                                   szAppName, MB_OK | MB_ICONEXCLAMATION) ;
                         return 0 ;
                    }
               break ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDM_NEW:
                    case IDM_OPEN:
                    case IDM_SAVE:
                    case IDM_SAVEAS:
                    case IDM_UNDO:
                    case IDM_CUT:
                    case IDM_COPY:
                    case IDM_PASTE:
                    case IDM_DEL:
                         MessageBeep (0) ;
                         return 0 ;

                    case IDM_COUR:
                    case IDM_ARIAL:
                    case IDM_TIMES:
                         hMenu = GetMenu (hwnd) ;
                         CheckMenuItem (hMenu, nCurrentFont, MF_UNCHECKED) ;
                         nCurrentFont = wParam ;
                         CheckMenuItem (hMenu, nCurrentFont, MF_CHECKED) ;
                         return 0 ;
                    }
               break ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
