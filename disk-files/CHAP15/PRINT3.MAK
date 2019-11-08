#----------------------
# PRINT3.EXE make file
#----------------------

print3.exe : print.obj print3.obj print3.def print.res
     $(WINLINK) print3 print, print3, NUL, $(WINLIB), print3
     rc -t print.res print3.exe

print.obj : print.c
     $(WINCC) print.c

print3.obj : print3.c
     $(WINCC) print3.c

print.res : print.rc
     $(WINRC) print.rc
