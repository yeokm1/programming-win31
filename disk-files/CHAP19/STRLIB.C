/*------------------------------------------------
   STRLIB.C -- Library module for STRPROG program
               (c) Charles Petzold,  1992
  ------------------------------------------------*/

#include <windows.h>
#include "strlib.h"

extern "C" {
int FAR PASCAL _export WEP (int) ;
}

HANDLE hStrings [256] ;
short  nTotal = 0 ;

int FAR PASCAL LibMain (HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
     {
     if (wHeapSize > 0)
          UnlockData (0) ;

     return 1 ;
     }

int FAR PASCAL _export WEP (int nParam)
     {
     return 1 ;
     }

BOOL FAR PASCAL _export AddString (LPSTR lpStringIn)
     {
     HANDLE hString ;
     NPSTR  npString ;
     short  i, nLength, nCompare ;

     if (nTotal == 255)
          return FALSE ;

     if (0 == (nLength = lstrlen (lpStringIn)))
          return FALSE ;

     if (NULL == (hString = LocalAlloc (LHND, 1 + nLength)))
          return FALSE ;

     npString = LocalLock (hString) ;
     lstrcpy (npString, lpStringIn) ;
     AnsiUpper (npString) ;
     LocalUnlock (hString) ;

     for (i = nTotal ; i > 0 ; i--)
          {
          npString = LocalLock (hStrings [i - 1]) ;
          nCompare = lstrcmpi (lpStringIn, npString) ;
          LocalUnlock (hStrings [i - 1]) ;

          if (nCompare > 0)
               {
               hStrings [i] = hString ;
               break ;
               }
          hStrings [i] = hStrings [i - 1] ;
          }

     if (i == 0)
          hStrings [0] = hString ;

     nTotal++ ;
     return TRUE ;
     }

BOOL FAR PASCAL _export DeleteString (LPSTR lpStringIn)
     {
     NPSTR npString ;
     short i, j, nCompare ;

     if (0 == lstrlen (lpStringIn))
          return FALSE ;

     for (i = 0 ; i < nTotal ; i++)
          {
          npString = LocalLock (hStrings [i]) ;
          nCompare = lstrcmpi (npString, lpStringIn) ;
          LocalUnlock (hStrings [i]) ;

          if (nCompare == 0)
               break ;
          }

     if (i == nTotal)
          return FALSE ;

     for (j = i ; j < nTotal ; j++)
          hStrings [j] = hStrings [j + 1] ;

     nTotal-- ;
     return TRUE ;
     }

short FAR PASCAL _export GetStrings (FPSTRCB lpfnGetStrCallBack, LPVOID lpParam)
     {
     BOOL  bReturn ;
     NPSTR npString ;
     short i ;

     for (i = 0 ; i < nTotal ; i++)
          {
          npString = LocalLock (hStrings [i]) ;
          bReturn = lpfnGetStrCallBack (npString, lpParam) ;
          LocalUnlock (hStrings [i]) ;

          if (bReturn == FALSE)
               return i + 1 ;
          }
     return nTotal ;
     }
