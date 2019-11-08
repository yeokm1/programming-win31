#-----------------------
# ENVIRON.MAK make file
#-----------------------

environ.exe : environ.obj environ.def
     $(WINLINK) environ, environ, NUL, $(WINLIB), environ
     rc -t environ.exe

environ.obj : environ.c
     $(WINCC) environ.c
