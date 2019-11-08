#------------------------
# SHOWPOP1.MAK make file
#------------------------

showpop1.exe : showpop1.obj showpop1.def
     $(WINLINK) showpop1, showpop1, NUL, $(WINLIB), showpop1
     rc -t showpop1.exe

showpop1.obj : showpop1.c showpop.h
     $(WINCC) showpop1.c
