#--------------------
# TYPE.MAK make file
#--------------------

type.exe : type.obj type.def
     $(WINLINK) type, type, NUL, $(WINLIB), type
     rc -t type.exe

type.obj : type.c
     $(WINCC) type.c
