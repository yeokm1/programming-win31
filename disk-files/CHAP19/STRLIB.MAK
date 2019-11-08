#----------------------
# STRLIB.MAK make file
#----------------------

strlib.dll : strlib.obj strlib.def
     $(DLLLINK) strlib, strlib.dll, NUL, $(DLLLIB), strlib
     rc strlib.dll

strlib.obj : strlib.c strlib.h
     $(DLLCC) strlib.c
