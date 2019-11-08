#-----------------------
# SHOWDIB.MAK make file
#-----------------------

showdib.exe : showdib.obj showdib.def showdib.res
     $(WINLINK) showdib, showdib, NUL, $(WINLIB), showdib
     rc -t showdib.res

showdib.obj : showdib.c showdib.h
     $(WINCC) showdib.c

showdib.res : showdib.rc showdib.h
     $(WINRC) showdib.rc
