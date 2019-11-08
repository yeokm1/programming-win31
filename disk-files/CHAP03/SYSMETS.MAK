#-----------------------
# SYSMETS.MAK make file
#-----------------------

sysmets.exe : sysmets.obj sysmets.def
     $(WINLINK) sysmets, sysmets, NUL, $(WINLIB), sysmets
     rc -t sysmets.exe

sysmets.obj : sysmets.c sysmets.h
     $(WINCC) sysmets.c
