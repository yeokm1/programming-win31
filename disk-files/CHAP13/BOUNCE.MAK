#----------------------
# BOUNCE.MAK make file
#----------------------

bounce.exe : bounce.obj bounce.def
     $(WINLINK) bounce, bounce, NUL, $(WINLIB), bounce
     rc -t bounce.exe

bounce.obj : bounce.c
     $(WINCC) bounce.c
