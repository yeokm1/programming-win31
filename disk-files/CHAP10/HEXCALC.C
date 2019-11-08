/*----------------------------------------
   HEXCALC.C -- Hexadecimal Calculator
                (c) Charles Petzold, 1992
  ----------------------------------------*/

#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

long FAR PASCAL _export WndProc (HWND, UINT, UINT, LONG) ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     static char szAppName [] = "HexCalc" ;
     HWND        hwnd ;
     MSG         msg;
     WNDCLASS    wndclass ;

     if (!hPrevInstance)
          {
          wndclass.style          = CS_HREDRAW | CS_VREDRAW;
          wndclass.lpfnWndProc    = WndProc ;
          wndclass.cbClsExtra     = 0 ;
          wndclass.cbWndExtra     = DLGWINDOWEXTRA ;
          wndclass.hInstance      = hInstance ;
          wndclass.hIcon          = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor        = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground  = COLOR_WINDOW + 1 ;
          wndclass.lpszMenuName   = NULL ;
          wndclass.lpszClassName  = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateDialog (hInstance, szAppName, 0, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          TranslateMessage (&msg) ;
          DispatchMessage (&msg) ;
          }
     return msg.wParam ;
     }

void ShowNumber (HWND hwnd, DWORD dwNumber)
     {
     char szBuffer [20] ;

     SetDlgItemText (hwnd, VK_ESCAPE, strupr (ltoa (dwNumber, szBuffer, 16))) ;
     }

DWORD CalcIt (DWORD dwFirstNum, short nOperation, DWORD dwNum)
     {
     switch (nOperation)
          {
          case '=' : return dwNum ;
          case '+' : return dwFirstNum +  dwNum ;
          case '-' : return dwFirstNum -  dwNum ;
          case '*' : return dwFirstNum *  dwNum ;
          case '&' : return dwFirstNum &  dwNum ;
          case '|' : return dwFirstNum |  dwNum ;
          case '^' : return dwFirstNum ^  dwNum ;
          case '<' : return dwFirstNum << dwNum ;
          case '>' : return dwFirstNum >> dwNum ;
          case '/' : return dwNum ? dwFirstNum / dwNum : ULONG_MAX ;
          case '%' : return dwNum ? dwFirstNum % dwNum : ULONG_MAX ;
          default  : return 0L ;
          }
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static BOOL  bNewNumber = TRUE ;
     static DWORD dwNumber, dwFirstNum ;
     static short nOperation = '=' ;
     HWND         hButton ;

     switch (message)
          {
          case WM_KEYDOWN:                   // left arrow --> backspace
               if (wParam != VK_LEFT)
                    break ;
               wParam = VK_BACK ;
                                             // fall through
          case WM_CHAR:
               if ((wParam = toupper (wParam)) == VK_RETURN)
                    wParam = '=' ;

               hButton = GetDlgItem (hwnd, wParam) ;

               if (hButton != NULL)
                    {
                    SendMessage (hButton, BM_SETSTATE, 1, 0L) ;
                    SendMessage (hButton, BM_SETSTATE, 0, 0L) ;
                    }
               else
                    {
                    MessageBeep (0) ;
                    break ;
                    }
                                             // fall through
          case WM_COMMAND:
               SetFocus (hwnd) ;

               if (wParam == VK_BACK)                  // backspace
                    ShowNumber (hwnd, dwNumber /= 16) ;

               else if (wParam == VK_ESCAPE)           // escape
                    ShowNumber (hwnd, dwNumber = 0L) ;

               else if (isxdigit (wParam))             // hex digit
                    {
                    if (bNewNumber)
                         {
                         dwFirstNum = dwNumber ;
                         dwNumber = 0L ;
                         }
                    bNewNumber = FALSE ;

                    if (dwNumber <= ULONG_MAX >> 4)
                         ShowNumber (hwnd, dwNumber = 16 * dwNumber + wParam -
                              (isdigit (wParam) ? '0' : 'A' - 10)) ;
                    else
                         MessageBeep (0) ;
                    }
               else                                    // operation
                    {
                    if (!bNewNumber)
                         ShowNumber (hwnd, dwNumber =
                              CalcIt (dwFirstNum, nOperation, dwNumber)) ;
                    bNewNumber = TRUE ;
                    nOperation = wParam ;
                    }
               return 0 ;

          case WM_DESTROY:
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }
