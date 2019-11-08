#-----------------------
# CONNECT.MAK make file
#-----------------------

connect.exe : connect.obj connect.def
     $(WINLINK) connect, connect, NUL, $(WINLIB), connect
     rc -t connect.exe

connect.obj : connect.c
     $(WINCC) connect.c
