#------------------------
# SYSMETS3.MAK make file
#------------------------

sysmets3.exe : sysmets3.obj sysmets3.def
     $(WINLINK) sysmets3, sysmets3, NUL, $(WINLIB), sysmets3
     rc -t sysmets3.exe

sysmets3.obj : sysmets3.c sysmets.h
     $(WINCC) sysmets3.c
