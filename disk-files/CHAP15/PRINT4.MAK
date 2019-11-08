#----------------------
# PRINT4.MAK make file
#----------------------

print4.exe : print.obj print4.obj print4.def print.res
     $(WINLINK) print4 print, print4, NUL, $(WINLIB), print4
     rc -t print.res print4.exe

print.obj : print.c
     $(WINCC) print.c

print4.obj : print4.c
     $(WINCC) print4.c

print.res : print.rc
     $(WINRC) print.rc
