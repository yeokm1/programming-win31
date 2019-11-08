#-----------------------
# JUSTIFY.MAK make file
#-----------------------

justify.exe : justify.obj justify.def justify.res
     $(WINLINK) justify, justify, NUL, $(WINLIB), justify
     rc -t justify.res

justify.obj : justify.c justify.h
     $(WINCC) justify.c

justify.res : justify.rc justify.asc justify.h
     $(WINRC) justify.rc
