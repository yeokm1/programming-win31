#-----------------------
# STRPROG.MAK make file
#-----------------------

strprog.exe : strprog.obj strprog.res strprog.def
     $(WINLINK) strprog, strprog, NUL, $(WINLIB), strprog
     rc strprog.res

strprog.obj : strprog.c strprog.h strlib.h
     $(WINCC) strprog.c

strprog.res : strprog.rc strprog.h
     $(WINRC) strprog.rc
