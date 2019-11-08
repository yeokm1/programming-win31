/*---------------------------------------
   POPPAD.C -- Popup Editor
               (c) Charles Petzold, 1992
  ---------------------------------------*/

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include "poppad.h"

#define EDITID   1
#define UNTITLED "(untitled)"

long FAR PASCAL _export WndProc      (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL _export AboutDlgProc (HWND, UINT, UINT, LONG) ;

          // Functions in POPFILE.C

void PopFileInitialize (HWND) ;
BOOL PopFileOpenDlg    (HWND, LPSTR, LPSTR) ;
BOOL PopFileSaveDlg    (HWND, LPSTR, LPSTR) ;
BOOL PopFileRead       (HWND, LPSTR) ;
BOOL PopFileWrite      (HWND, LPSTR) ;

          // Functions in POPFIND.C

HWND PopFindFindDlg     (HWND) ;
HWND PopFindReplaceDlg  (HWND) ;
BOOL PopFindFindText    (HWND, int *, LPFINDREPLACE) ;
BOOL PopFindReplaceText (HWND, int *, LPFINDREPLACE) ;
BOOL PopFindNextText    (HWND, int *) ;
BOOL PopFindValidFind   (void) ;

          // Functions in POPFONT.C

void PopFontInitialize   (HWND) ;
BOOL PopFontChooseFont   (HWND) ;
void PopFontSetFont      (HWND) ;
void PopFontDeinitialize (void) ;

          // Functions in POPPRNT.C

BOOL PopPrntPrintFile (HANDLE, HWND, HWND, LPSTR) ;

          // Global variables

static char szAppName [] = "PopPad" ;
static HWND hDlgModeless ;

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     MSG      msg;
     HWND     hwnd ;
     HANDLE   hAccel ;
     WNDCLASS wndclass ;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, szAppName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

     hwnd = CreateWindow (szAppName, NULL,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, lpszCmdLine) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd); 

     hAccel = LoadAccelerators (hInstance, szAppName) ;

     while (GetMessage (&msg, NULL, 0, 0))
          {
          if (hDlgModeless == NULL || !IsDialogMessage (hDlgModeless, &msg))
               {
               if (!TranslateAccelerator (hwnd, hAccel, &msg))
                    {
                    TranslateMessage (&msg) ;
                    DispatchMessage (&msg) ;
                    }
               }
          }
     return msg.wParam ;
     }

void DoCaption (HWND hwnd, char *szTitleName)
     {
     char szCaption [64 + _MAX_FNAME + _MAX_EXT] ;

     wsprintf (szCaption, "%s - %s", (LPSTR) szAppName,
               (LPSTR) (szTitleName [0] ? szTitleName : UNTITLED)) ;

     SetWindowText (hwnd, szCaption) ;
     }

void OkMessage (HWND hwnd, char *szMessage, char *szTitleName)
     {
     char szBuffer [64 + _MAX_FNAME + _MAX_EXT] ;

     wsprintf (szBuffer, szMessage,
               (LPSTR) (szTitleName [0] ? szTitleName : UNTITLED)) ;

     MessageBox (hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION) ;
     }

short AskAboutSave (HWND hwnd, char *szTitleName)
     {
     char  szBuffer [64 + _MAX_FNAME + _MAX_EXT] ;
     short nReturn ;

     wsprintf (szBuffer, "Save current changes in %s?",
               (LPSTR) (szTitleName [0] ? szTitleName : UNTITLED)) ;

     nReturn = MessageBox (hwnd, szBuffer, szAppName,
                           MB_YESNOCANCEL | MB_ICONQUESTION) ;

     if (nReturn == IDYES)
          if (!SendMessage (hwnd, WM_COMMAND, IDM_SAVE, 0L))
               nReturn = IDCANCEL ;

     return nReturn ;
     }

long FAR PASCAL _export WndProc (HWND hwnd, UINT message, UINT wParam,
                                                          LONG lParam)
     {
     static BOOL    bNeedSave = FALSE ;
     static char    szFileName  [_MAX_PATH] ;
     static char    szTitleName [_MAX_FNAME + _MAX_EXT] ;
     static FARPROC lpfnAboutDlgProc ;
     static HANDLE  hInst ;
     static HWND    hwndEdit ;
     static int     iOffset ;
     static UINT    messageFindReplace ;
     LONG           lSelect ;
     LPFINDREPLACE  lpfr ;
     WORD           wEnable ;

     switch (message)
          {
          case WM_CREATE:
                         // Get About dialog instance address

               hInst = ((LPCREATESTRUCT) lParam)->hInstance ;
               lpfnAboutDlgProc = MakeProcInstance ((FARPROC) AboutDlgProc,
                                                    hInst) ;

                         // Create the edit control child window

               hwndEdit = CreateWindow ("edit", NULL,
                         WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
                              WS_BORDER | ES_LEFT | ES_MULTILINE |
                              ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
                         0, 0, 0, 0,
                         hwnd, EDITID, hInst, NULL) ;

               SendMessage (hwndEdit, EM_LIMITTEXT, 32000, 0L) ;

                         // Initialize common dialog box stuff

               PopFileInitialize (hwnd) ;
               PopFontInitialize (hwndEdit) ;

               messageFindReplace = RegisterWindowMessage (FINDMSGSTRING) ;

                         // Process command line

               lstrcpy (szFileName, (LPSTR)
                        (((LPCREATESTRUCT) lParam)->lpCreateParams)) ;

               if (lstrlen (szFileName) > 0)
                    {
                    GetFileTitle (szFileName, szTitleName,
                                  sizeof (szTitleName)) ;

                    if (!PopFileRead (hwndEdit, szFileName))
                         OkMessage (hwnd, "File %s cannot be read!",
                                          szTitleName) ;
                    }

               DoCaption (hwnd, szTitleName) ;
               return 0 ;

          case WM_SETFOCUS:
               SetFocus (hwndEdit) ;
               return 0 ;

          case WM_SIZE: 
               MoveWindow (hwndEdit, 0, 0, LOWORD (lParam),
                                           HIWORD (lParam), TRUE) ;
               return 0 ;

          case WM_INITMENUPOPUP:
               switch (lParam)
                    {
                    case 1:        // Edit menu

                              // Enable Undo if edit control can do it

                         EnableMenuItem (wParam, IDM_UNDO,
                              SendMessage (hwndEdit, EM_CANUNDO, 0, 0L) ?
                                   MF_ENABLED : MF_GRAYED) ;

                              // Enable Paste if text is in the clipboard

                         EnableMenuItem (wParam, IDM_PASTE,
                              IsClipboardFormatAvailable (CF_TEXT) ?
                                   MF_ENABLED : MF_GRAYED) ;

                              // Enable Cut, Copy, and Del if text is selected

                         lSelect = SendMessage (hwndEdit, EM_GETSEL, 0, 0L) ;
                         wEnable = HIWORD (lSelect) != LOWORD (lSelect) ?
                                        MF_ENABLED : MF_GRAYED ;

                         EnableMenuItem (wParam, IDM_CUT,  wEnable) ;
                         EnableMenuItem (wParam, IDM_COPY, wEnable) ;
                         EnableMenuItem (wParam, IDM_DEL,  wEnable) ;
                         break ;

                    case 2:        // Search menu

                              // Enable Find, Next, and Replace if modeless
                              //   dialogs are not already active

                         wEnable = hDlgModeless == NULL ?
                                        MF_ENABLED : MF_GRAYED ;

                         EnableMenuItem (wParam, IDM_FIND,    wEnable) ;
                         EnableMenuItem (wParam, IDM_NEXT,    wEnable) ;
                         EnableMenuItem (wParam, IDM_REPLACE, wEnable) ;
                         break ;
                    }
               return 0 ;

          case WM_COMMAND :
                              // Messages from edit control

               if (LOWORD (lParam) && wParam == EDITID)
                    {
                    switch (HIWORD (lParam))
                         {
                         case EN_UPDATE:
                              bNeedSave = TRUE ;
                              return 0 ;

                         case EN_ERRSPACE:
                         case EN_MAXTEXT:
                              MessageBox (hwnd, "Edit control out of space.",
                                        szAppName, MB_OK | MB_ICONSTOP) ;
                              return 0 ;
                         }
                    break ;
                    }

               switch (wParam)
                    {
                              // Messages from File menu

                    case IDM_NEW:
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szTitleName))
                              return 0 ;

                         SetWindowText (hwndEdit, "\0") ;
                         szFileName [0]  = '\0' ;
                         szTitleName [0] = '\0' ;
                         DoCaption (hwnd, szTitleName) ;
                         bNeedSave = FALSE ;
                         return 0 ;

                    case IDM_OPEN:
                         if (bNeedSave && IDCANCEL ==
                                   AskAboutSave (hwnd, szTitleName))
                              return 0 ;

                         if (PopFileOpenDlg (hwnd, szFileName, szTitleName))
                              {
                              if (!PopFileRead (hwndEdit, szFileName))
                                   {
                                   OkMessage (hwnd, "Could not read file %s!",
                                                    szTitleName) ;
                                   szFileName  [0] = '\0' ;
                                   szTitleName [0] = '\0' ;
                                   }
                              }

                         DoCaption (hwnd, szTitleName) ;
                         bNeedSave = FALSE ;
                         return 0 ;

                    case IDM_SAVE:
                         if (szFileName [0])
                              {
                              if (PopFileWrite (hwndEdit, szFileName))
                                   {
                                   bNeedSave = FALSE ;
                                   return 1 ;
                                   }
                              else
                                   OkMessage (hwnd, "Could not write file %s",
                                                    szTitleName) ;
                              return 0 ;
                              }
                                                  // fall through
                    case IDM_SAVEAS:
                         if (PopFileSaveDlg (hwnd, szFileName, szTitleName))
                              {
                              DoCaption (hwnd, szTitleName) ;

                              if (PopFileWrite (hwndEdit, szFileName))
                                   {
                                   bNeedSave = FALSE ;
                                   return 1 ;
                                   }
                              else
                                   OkMessage (hwnd, "Could not write file %s",
                                                    szTitleName) ;
                              }
                         return 0 ;

                    case IDM_PRINT:
                         if (!PopPrntPrintFile (hInst, hwnd, hwndEdit,
                                                szTitleName))
                              OkMessage (hwnd, "Could not print file %s",
                                         szTitleName) ;
                         return 0 ;

                    case IDM_EXIT:
                         SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                         return 0 ;

                              // Messages from Edit menu

                    case IDM_UNDO:
                         SendMessage (hwndEdit, WM_UNDO, 0, 0L) ;
                         return 0 ;

                    case IDM_CUT:
                         SendMessage (hwndEdit, WM_CUT, 0, 0L) ;
                         return 0 ;

                    case IDM_COPY:
                         SendMessage (hwndEdit, WM_COPY, 0, 0L) ;
                         return 0 ;

                    case IDM_PASTE:
                         SendMessage (hwndEdit, WM_PASTE, 0, 0L) ;
                         return 0 ;

                    case IDM_DEL:
                         SendMessage (hwndEdit, WM_CLEAR, 0, 0L) ;
                         return 0 ;

                    case IDM_SELALL:
                         SendMessage (hwndEdit, EM_SETSEL, 0,
                                        MAKELONG (0, 32767)) ;
                         return 0 ;

                              // Messages from Search menu

                    case IDM_FIND:
                         iOffset = HIWORD (
                              SendMessage (hwndEdit, EM_GETSEL, 0, 0L)) ;
                         hDlgModeless = PopFindFindDlg (hwnd) ;
                         return 0 ;

                    case IDM_NEXT:
                         iOffset = HIWORD (
                              SendMessage (hwndEdit, EM_GETSEL, 0, 0L)) ;

                         if (PopFindValidFind ())
                              PopFindNextText (hwndEdit, &iOffset) ;
                         else
                              hDlgModeless = PopFindFindDlg (hwnd) ;

                         return 0 ;

                    case IDM_REPLACE:
                         iOffset = HIWORD (
                              SendMessage (hwndEdit, EM_GETSEL, 0, 0L)) ;

                         hDlgModeless = PopFindReplaceDlg (hwnd) ;
                         return 0 ;

                    case IDM_FONT:
                         if (PopFontChooseFont (hwnd))
                              PopFontSetFont (hwndEdit) ;

                         return 0 ;

                              // Messages from Help menu

                    case IDM_HELP:
                         OkMessage (hwnd, "Help not yet implemented!", NULL) ;
                         return 0 ;

                    case IDM_ABOUT:
                         DialogBox (hInst, "AboutBox", hwnd, lpfnAboutDlgProc);
                         return 0 ;
                    }
               break ;

          case WM_CLOSE:
               if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
                    DestroyWindow (hwnd) ;

               return 0 ;

          case WM_QUERYENDSESSION:
               if (!bNeedSave || IDCANCEL != AskAboutSave (hwnd, szTitleName))
                    return 1L ;

               return 0 ;

          case WM_DESTROY:
               PopFontDeinitialize () ;
               PostQuitMessage (0) ;
               return 0 ;

          default:
                         // Process "Find-Replace" messages

               if (message == messageFindReplace)
                    {
                    lpfr = (LPFINDREPLACE) lParam ;

                    if (lpfr->Flags & FR_DIALOGTERM)
                         hDlgModeless = NULL ;

                    if (lpfr->Flags & FR_FINDNEXT)
                         if (!PopFindFindText (hwndEdit, &iOffset, lpfr))
                              OkMessage (hwnd, "Text not found!", NULL) ;

                    if (lpfr->Flags & FR_REPLACE ||
                        lpfr->Flags & FR_REPLACEALL)
                         if (!PopFindReplaceText (hwndEdit, &iOffset, lpfr))
                              OkMessage (hwnd, "Text not found!", NULL) ;

                    if (lpfr->Flags & FR_REPLACEALL)
                         while (PopFindReplaceText (hwndEdit, &iOffset, lpfr));

                    return 0 ;
                    }
               break ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
     }

BOOL FAR PASCAL _export AboutDlgProc (HWND hDlg, UINT message, UINT wParam,
                                                               LONG lParam)
     {
     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                         EndDialog (hDlg, 0) ;
                         return TRUE ;
                    }
               break ;
          }
     return FALSE ;
     }
