/*------------------------------------------------
   COLORS2.C -- Version using Modeless Dialog Box
                (c) Charles Petzold, 1992
  ------------------------------------------------*/

#include <windows.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export ColorScrDlg (HWND, UINT, UINT, LONG) ;

HWND hDlgModeless ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName[] = "Colors2" ;
     HWND        hwnd ;
     MSG         msg;
     WNDCLASS    wndclass ;

     if (hPrevInstance)
          return FALSE ;

     wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
     wndclass.lpfnWndProc   = WndProc ;
     wndclass.cbClsExtra    = 0 ;
     wndclass.cbWndExtra    = 0 ;
     wndclass.hInstance     = hInstance ;
     wndclass.hIcon         = NULL ;
     wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
     wndclass.hbrBackground = CreateSolidBrush (0L) ;
     wndclass.lpszMenuName  = NULL ;
     wndclass.lpszClassName = szAppName ;

     RegisterClass (&wndclass) ;

     hwnd = CreateWindow (szAppName, "Color Scroll",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd);

     hDlgModeless = CreateDialog (hInstance, "ColorScrDlg", hwnd,
                         MakeProcInstance ((FARPROC) ColorScrDlg, hInstance)) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          if (hDlgModeless == 0 || !IsDialogMessage (hDlgModeless, &msg))
               {
               TranslateMessage (&msg) ;
               DispatchMessage  (&msg) ;
               }
          }
     return msg.wParam ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     switch (message)
          {
          case WM_DESTROY:
               DeleteObject (
                    SetClassWord (hwnd, GCW_HBRBACKGROUND,
                         GetStockObject (WHITE_BRUSH))) ;

               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

BOOL FAR PASCAL _export ColorScrDlg (HWND hDlg, UINT message, UINT wParam,
                                                              LONG lParam)
     {
     static short color [3] ;
     HWND         hwndParent, hCtrl ;
     short        nCtrlID, nIndex ;

     switch (message)
          {
          case WM_INITDIALOG:
               for (nCtrlID = 10 ; nCtrlID < 13 ; nCtrlID++)
                    {
                    hCtrl = GetDlgItem (hDlg, nCtrlID) ;
                    SetScrollRange (hCtrl, SB_CTL, 0, 255, FALSE) ;
                    SetScrollPos   (hCtrl, SB_CTL, 0, FALSE) ;
                    }
               return TRUE ;

          case WM_VSCROLL :
               hCtrl   = HIWORD (lParam) ;
               nCtrlID = GetWindowWord (hCtrl, GWW_ID) ;
               nIndex  = nCtrlID - 10 ;
               hwndParent = GetParent (hDlg) ;

               switch (wParam)
                    {
                    case SB_PAGEDOWN :
                         color [nIndex] += 15 ;        // fall through
                    case SB_LINEDOWN :
                         color [nIndex] = min (255, color [nIndex] + 1) ;
                         break ;
                    case SB_PAGEUP :
                         color [nIndex] -= 15 ;        // fall through
                    case SB_LINEUP :
                         color [nIndex] = max (0, color [nIndex] - 1) ;
                         break ;
                    case SB_TOP:
                         color [nIndex] = 0 ;
                         break ;
                    case SB_BOTTOM :
                         color [nIndex] = 255 ;
                         break ;
                    case SB_THUMBPOSITION :
                    case SB_THUMBTRACK :
                         color [nIndex] = LOWORD (lParam) ;
                         break ;
                    default :
                         return FALSE ;
                    }
               SetScrollPos  (hCtrl, SB_CTL,      color [nIndex], TRUE) ;
               SetDlgItemInt (hDlg,  nCtrlID + 3, color [nIndex], FALSE) ;

               DeleteObject (
                    SetClassWord (hwndParent, GCW_HBRBACKGROUND,
                         CreateSolidBrush (
                              RGB (color[0], color[1], color[2])))) ;

               InvalidateRect (hwndParent, NULL, TRUE) ;
               return TRUE ;
          }
     return FALSE ;
     }
