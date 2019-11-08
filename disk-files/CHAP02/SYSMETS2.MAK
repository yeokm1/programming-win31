#------------------------
# SYSMETS2.MAK make file
#------------------------

sysmets2.exe : sysmets2.obj sysmets2.def
     $(WINLINK) sysmets2, sysmets2, NUL, $(WINLIB), sysmets2
     rc -t sysmets2.exe

sysmets2.obj : sysmets2.c sysmets.h
     $(WINCC) sysmets2.c
