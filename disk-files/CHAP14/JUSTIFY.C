/*----------------------------------------
   JUSTIFY.C -- Justified Type Program
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include "justify.h"

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static   char szAppName[] = "Justify" ;
     HWND     hwnd ;
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
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }
     hwnd = CreateWindow (szAppName, "Justified Type",
                          WS_OVERLAPPEDWINDOW,
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

void DrawRuler (HDC hdc, RECT *prc)
     {
     static short nRuleSize [16] = { 360, 72, 144, 72, 216, 72, 144, 72,
                                     288, 72, 144, 72, 216, 72, 144, 72 } ;
     short        i, j ;
     POINT        ptClient ;

     SaveDC (hdc) ;
                    // Set Logical Twips mapping mode

     SetMapMode (hdc, MM_ANISOTROPIC) ;
     SetWindowExt (hdc, 1440, 1440) ;
     SetViewportExt (hdc, GetDeviceCaps (hdc, LOGPIXELSX),
                          GetDeviceCaps (hdc, LOGPIXELSY)) ;

                    // Move the origin to a half inch from upper left

     SetWindowOrg (hdc, -720, -720) ;

                    // Find the right margin (quarter inch from right)

     ptClient.x = prc->right ;
     ptClient.y = prc->bottom ;
     DPtoLP (hdc, &ptClient, 1) ;
     ptClient.x -= 360 ;

                    // Draw the rulers

     MoveTo (hdc, 0,          -360) ;
     LineTo (hdc, ptClient.x, -360) ;
     MoveTo (hdc, -360,          0) ;
     LineTo (hdc, -360, ptClient.y) ;

     for (i = 0, j = 0 ; i <= ptClient.x ; i += 1440 / 16, j++)
          {
          MoveTo (hdc, i, -360) ;
          LineTo (hdc, i, -360 - nRuleSize [j % 16]) ;
          }
     for (i = 0, j = 0 ; i <= ptClient.y ; i += 1440 / 16, j++)
          {
          MoveTo (hdc, -360, i) ;
          LineTo (hdc, -360 - nRuleSize [j % 16], i) ;
          }

     RestoreDC (hdc, -1) ;
     }

void Justify (HDC hdc, LPSTR lpText, RECT *prc, short nAlign)
     {
     DWORD  dwExtent ;
     LPSTR  lpBegin, lpEnd ;
     short  xStart, yStart, nBreakCount ;

     yStart = prc->top ;
     do                            // for each text line
          {
          nBreakCount = 0 ;
          while (*lpText == ' ')   // skip over leading blanks
               lpText++ ;
          lpBegin = lpText ;

          do                       // until the line is known
               {
               lpEnd = lpText ;

               while (*lpText != '\0' && *lpText++ != ' ') ;
               if (*lpText == '\0')
                    break ;
                                   // for each space, calculate extents
               nBreakCount++ ;
               SetTextJustification (hdc, 0, 0) ;
               dwExtent = GetTextExtent (hdc, lpBegin, lpText - lpBegin - 1) ;
               }
          while ((int) LOWORD (dwExtent) < (prc->right - prc->left)) ;

          nBreakCount-- ;
          while (*(lpEnd - 1) == ' ')   // eliminate trailing blanks
               {
               lpEnd-- ;
               nBreakCount-- ;
               }

          if (*lpText == '\0' || nBreakCount <= 0)
               lpEnd = lpText ;

          SetTextJustification (hdc, 0, 0) ;
          dwExtent = GetTextExtent (hdc, lpBegin, lpEnd - lpBegin) ;


          switch (nAlign)               // use alignment for xStart
               {
               case IDM_LEFT:
                    xStart = prc->left ;
                    break ;

               case IDM_RIGHT:
                    xStart = prc->right - LOWORD (dwExtent) ;
                    break ;

               case IDM_CENTER:
                    xStart = (prc->right + prc->left - LOWORD (dwExtent)) / 2 ;
                    break ;

               case IDM_JUST:
                    if (*lpText != '\0' && nBreakCount > 0)
                         SetTextJustification (hdc,
                              prc->right - prc->left - LOWORD (dwExtent),
                              nBreakCount) ;
                    xStart = prc->left ;
                    break ;
               }

          TextOut (hdc, xStart, yStart, lpBegin, lpEnd - lpBegin) ;
          yStart += HIWORD (dwExtent) ;
          lpText = lpEnd ;
          }
     while (*lpText && yStart < prc->bottom) ;

     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static CHOOSEFONT cf ;
     static HANDLE     hResource ;
     static LOGFONT    lf ;
     static LPSTR      lpText ;
     static short      nAlign = IDM_LEFT ;
     HANDLE            hInstance ;
     HDC               hdc ;
     HMENU             hMenu ;
     PAINTSTRUCT       ps ;
     RECT              rcClient ;

     switch (message)
          {
          case WM_CREATE:
                              // Initialize the CHOOSEFONT structure

               GetObject (GetStockObject (SYSTEM_FONT), sizeof (LOGFONT),
                          (LPSTR) &lf) ;

               cf.lStructSize      = sizeof (CHOOSEFONT) ;
               cf.hwndOwner        = hwnd ;
               cf.lpLogFont        = &lf ;
               cf.Flags            = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS
                                                            | CF_EFFECTS ;
                              // Load the Text Resource

               hInstance = ((LPCREATESTRUCT) lParam)-> hInstance ;

               hResource = LoadResource (hInstance, 
                           FindResource (hInstance, "Ismael", "TEXT")) ;

               lpText = LockResource (hResource) ;
               return 0 ;

          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_FONT:
                         if (ChooseFont (&cf))
                              InvalidateRect (hwnd, NULL, TRUE) ;
                         break ;

                    case IDM_LEFT:
                    case IDM_RIGHT:
                    case IDM_CENTER:
                    case IDM_JUST:
                         CheckMenuItem (hMenu, nAlign, MF_UNCHECKED) ;
                         nAlign = wParam ;
                         CheckMenuItem (hMenu, nAlign, MF_CHECKED) ;
                         InvalidateRect (hwnd, NULL, TRUE) ;
                         break ;
                    }
               return 0 ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               GetClientRect (hwnd, &rcClient) ;
               DrawRuler (hdc, &rcClient) ;

               rcClient.left  += GetDeviceCaps (hdc, LOGPIXELSX) / 2 ;
               rcClient.top   += GetDeviceCaps (hdc, LOGPIXELSY) / 2 ;
               rcClient.right -= GetDeviceCaps (hdc, LOGPIXELSX) / 4 ;

               SelectObject (hdc, CreateFontIndirect (&lf)) ;
               SetTextColor (hdc, cf.rgbColors) ;

               Justify (hdc, lpText, &rcClient, nAlign) ;

               DeleteObject (SelectObject (hdc, GetStockObject (SYSTEM_FONT)));
               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               GlobalUnlock (hResource) ;
               FreeResource (hResource) ;
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
