#--------------------
# HEAD.MAK make file
#--------------------

head.exe : head.obj head.def
     $(WINLINK) head, head, NUL, $(WINLIB), head
     rc -t head.exe

head.obj : head.c
     $(WINCC) head.c
