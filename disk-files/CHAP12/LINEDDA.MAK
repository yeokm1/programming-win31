#-----------------------
# LINEDDA.MAK make file
#-----------------------

linedda.exe : linedda.obj linedda.def
     $(WINLINK) linedda, linedda, NUL, $(WINLIB), linedda
     rc -t linedda.exe

linedda.obj : linedda.c
     $(WINCC) linedda.c
