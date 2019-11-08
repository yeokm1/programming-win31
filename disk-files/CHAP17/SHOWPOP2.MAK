#------------------------
# SHOWPOP2.MAK make file
#------------------------

showpop2.exe : showpop2.obj showpop2.def
     $(WINLINK) showpop2, showpop2, NUL, $(WINLIB), showpop2
     rc -t showpop2.exe

showpop2.obj : showpop2.c showpop.h
     $(WINCC) showpop2.c
