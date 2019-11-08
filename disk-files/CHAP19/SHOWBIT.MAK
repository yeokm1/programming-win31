#-----------------------
# SHOWBIT.MAK make file
#-----------------------

showbit.exe : showbit.obj showbit.def
     $(WINLINK) showbit, showbit, NUL, $(WINLIB), showbit
     rc showbit.exe

showbit.obj : showbit.c
     $(WINCC) showbit.c
