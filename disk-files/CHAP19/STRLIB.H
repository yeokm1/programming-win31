/*----------------------
   STRLIB.H header file
  ----------------------*/

typedef BOOL (FAR PASCAL _export * FPSTRCB) (LPSTR, LPVOID) ;

extern "C" {
BOOL  FAR PASCAL _export AddString    (LPSTR) ;
BOOL  FAR PASCAL _export DeleteString (LPSTR) ;
short FAR PASCAL _export GetStrings   (FPSTRCB, LPVOID) ;
}
