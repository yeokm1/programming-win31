#------------------------
# SYSMETS1.MAK make file
#------------------------

sysmets1.exe : sysmets1.obj sysmets1.def
     $(WINLINK) sysmets1, sysmets1, NUL, $(WINLIB), sysmets1
     rc -t sysmets1.exe

sysmets1.obj : sysmets1.c sysmets.h
     $(WINCC) sysmets1.c
