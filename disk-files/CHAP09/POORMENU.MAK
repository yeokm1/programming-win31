#------------------------
# POORMENU.MAK make file
#------------------------

poormenu.exe : poormenu.obj poormenu.def
     $(WINLINK) poormenu, poormenu, NUL, $(WINLIB), poormenu
     rc -t poormenu.exe

poormenu.obj : poormenu.c
     $(WINCC) poormenu.c
