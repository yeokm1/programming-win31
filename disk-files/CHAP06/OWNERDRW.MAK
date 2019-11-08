#------------------------
# OWNERDRW.MAK make file
#------------------------

ownerdrw.exe : ownerdrw.obj ownerdrw.def
     $(WINLINK) ownerdrw, ownerdrw, NUL, $(WINLIB), ownerdrw
     rc -t ownerdrw.exe

ownerdrw.obj : ownerdrw.c
     $(WINCC) ownerdrw.c
