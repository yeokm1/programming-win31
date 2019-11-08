#-----------------------
# POPMENU.MAK make file
#-----------------------

popmenu.exe : popmenu.obj popmenu.def popmenu.res
     $(WINLINK) popmenu, popmenu, NUL, $(WINLIB), popmenu
     rc -t popmenu.res

popmenu.obj : popmenu.c popmenu.h
     $(WINCC) popmenu.c

popmenu.res : popmenu.rc popmenu.h
     $(WINRC) popmenu.rc
