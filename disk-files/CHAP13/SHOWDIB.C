/*----------------------------------------
   SHOWDIB.C -- Show DIB in Window
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <stdlib.h>
#include "showdib.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

char szAppName [] = "ShowDib" ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND        hwnd ;
     MSG         msg ;
     WNDCLASS    wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = NULL ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, "Show DIB in Window",
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

          // Functions for extracting information from DIB memory blocks

DWORD GetDibInfoHeaderSize (BYTE huge * lpDib)
     {
     return ((BITMAPINFOHEADER huge *) lpDib)->biSize ;
     }

WORD GetDibWidth (BYTE huge * lpDib)
     {
     if (GetDibInfoHeaderSize (lpDib) == sizeof (BITMAPCOREHEADER))
          return (WORD) (((BITMAPCOREHEADER huge *) lpDib)->bcWidth) ;
     else
          return (WORD) (((BITMAPINFOHEADER huge *) lpDib)->biWidth) ;
     }

WORD GetDibHeight (BYTE huge * lpDib)
     {
     if (GetDibInfoHeaderSize (lpDib) == sizeof (BITMAPCOREHEADER))
          return (WORD) (((BITMAPCOREHEADER huge *) lpDib)->bcHeight) ;
     else
          return (WORD) (((BITMAPINFOHEADER huge *) lpDib)->biHeight) ;
     }

BYTE huge * GetDibBitsAddr (BYTE huge * lpDib)
     {
     DWORD dwNumColors, dwColorTableSize ;
     WORD  wBitCount ;

     if (GetDibInfoHeaderSize (lpDib) == sizeof (BITMAPCOREHEADER))
          {
          wBitCount = ((BITMAPCOREHEADER huge *) lpDib)->bcBitCount ;

          if (wBitCount != 24)
               dwNumColors = 1L << wBitCount ;
          else
               dwNumColors = 0 ;

          dwColorTableSize = dwNumColors * sizeof (RGBTRIPLE) ;
          }
     else
          {
          wBitCount = ((BITMAPINFOHEADER huge *) lpDib)->biBitCount ;

          if (GetDibInfoHeaderSize (lpDib) >= 36)
               dwNumColors = ((BITMAPINFOHEADER huge *) lpDib)->biClrUsed ;
          else
               dwNumColors = 0 ;

          if (dwNumColors == 0)
               {
               if (wBitCount != 24)
                    dwNumColors = 1L << wBitCount ;
               else
                    dwNumColors = 0 ;
               }

          dwColorTableSize = dwNumColors * sizeof (RGBQUAD) ;
          }

     return lpDib + GetDibInfoHeaderSize (lpDib) + dwColorTableSize ;
     }

          // Read a DIB from a file into memory

BYTE huge * ReadDib (char * szFileName)
     {
     BITMAPFILEHEADER bmfh ;
     BYTE huge *      lpDib ;
     DWORD            dwDibSize, dwOffset, dwHeaderSize ;
     int              hFile ;
     WORD             wDibRead ;

     if (-1 == (hFile = _lopen (szFileName, OF_READ | OF_SHARE_DENY_WRITE)))
          return NULL ;


     if (_lread (hFile, (LPSTR) &bmfh, sizeof (BITMAPFILEHEADER)) !=
                                       sizeof (BITMAPFILEHEADER))
          {
          _lclose (hFile) ;
          return NULL ;
          }

     if (bmfh.bfType != * (WORD *) "BM")
          {
          _lclose (hFile) ;
          return NULL ;
          }

     dwDibSize = bmfh.bfSize - sizeof (BITMAPFILEHEADER) ;

     lpDib = (BYTE huge * ) GlobalAllocPtr (GMEM_MOVEABLE, dwDibSize) ;

     if (lpDib == NULL)
          {
          _lclose (hFile) ;
          return NULL ;
          }

     dwOffset = 0 ;

     while (dwDibSize > 0)
          {
          wDibRead = (WORD) MIN (32768ul, dwDibSize) ;

          if (wDibRead != _lread (hFile, (LPSTR) (lpDib + dwOffset), wDibRead))
               {
               _lclose (hFile) ;
               GlobalFreePtr (lpDib) ;
               return NULL ;
               }

          dwDibSize -= wDibRead ;
          dwOffset  += wDibRead ;
          }

     _lclose (hFile) ;

     dwHeaderSize = GetDibInfoHeaderSize (lpDib) ;

     if (dwHeaderSize < 12 || (dwHeaderSize > 12 && dwHeaderSize < 16))
          {
          GlobalFreePtr (lpDib) ;
          return NULL ;
          }

     return lpDib ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static char         szFileName  [_MAX_PATH],
                         szTitleName [_MAX_FNAME + _MAX_EXT] ;
     static char *       szFilter[] = { "Bitmap Files (*.BMP)",  "*.bmp",
                                        "DIB Files (*.DIB)",     "*.dib",
                                        "" } ;
     static BYTE huge *  lpDib ;
     static OPENFILENAME ofn ;
     static short        cxClient, cyClient ;
     static WORD         wDisplay = IDM_ACTUAL ;
     HDC                 hdc ;
     HMENU               hMenu ;
     BYTE huge *         lpDibBits ;
     PAINTSTRUCT         ps ;
     short               cxDib, cyDib ;

     switch (message)
          {
          case WM_CREATE:
               ofn.lStructSize       = sizeof (OPENFILENAME) ;
               ofn.hwndOwner         = hwnd ;
               ofn.lpstrFilter       = szFilter [0] ;
               ofn.lpstrFile         = szFileName ;
               ofn.nMaxFile          = _MAX_PATH ;
               ofn.lpstrFileTitle    = szTitleName ;
               ofn.nMaxFileTitle     = _MAX_FNAME + _MAX_EXT ;
               ofn.lpstrDefExt       = "bmp" ;

               return 0 ;

          case WM_SIZE:
               cxClient = LOWORD (lParam) ;
               cyClient = HIWORD (lParam) ;
               return 0 ;

          case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_OPEN:
                         if (GetOpenFileName (&ofn))
                              {
                              if (lpDib != NULL)
                                   {
                                   GlobalFreePtr (lpDib) ;
                                   lpDib = NULL ;
                                   }

                              lpDib = ReadDib (szFileName) ;

                              if (lpDib == NULL)
                                   MessageBox (hwnd, szAppName,
                                               "Could not open DIB file",
                                               MB_ICONEXCLAMATION | MB_OK) ;

                              InvalidateRect (hwnd, NULL, TRUE) ;
                              }
                         return 0 ;

                    case IDM_ACTUAL:
                    case IDM_STRETCH:
                         CheckMenuItem (hMenu, wDisplay, MF_UNCHECKED) ;
                         wDisplay = wParam ;
                         CheckMenuItem (hMenu, wDisplay, MF_CHECKED) ;

                         InvalidateRect (hwnd, NULL, TRUE) ;
                         return 0 ;
                    }
               break ;

          case WM_PAINT:
               hdc = BeginPaint (hwnd, &ps) ;

               if (lpDib != NULL)
                    {
                    lpDibBits = GetDibBitsAddr (lpDib) ;
                    cxDib     = GetDibWidth    (lpDib) ;
                    cyDib     = GetDibHeight   (lpDib) ;

                    SetStretchBltMode (hdc, COLORONCOLOR) ;

                    if (wDisplay == IDM_ACTUAL)
                         SetDIBitsToDevice (hdc, 0, 0, cxDib, cyDib, 0, 0,
                                            0, cyDib, (LPSTR) lpDibBits,
                                            (LPBITMAPINFO) lpDib,
                                            DIB_RGB_COLORS) ;

                    else
                         StretchDIBits (hdc, 0, 0, cxClient, cyClient,
                                        0, 0, cxDib, cyDib,
                                        (LPSTR) lpDibBits,
                                        (LPBITMAPINFO) lpDib,
                                        DIB_RGB_COLORS, SRCCOPY) ;
                    }

               EndPaint (hwnd, &ps) ;
               return 0 ;

          case WM_DESTROY:
               if (lpDib != NULL)
                    GlobalFreePtr (lpDib) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
