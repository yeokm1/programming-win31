#----------------------
# PRINT2.MAK make file
#----------------------

print2.exe : print.obj print2.obj print2.def
     $(WINLINK) print2 print, print2, NUL, $(WINLIB), print2
     rc -t print2.exe

print.obj : print.c
     $(WINCC) print.c

print2.obj : print2.c
     $(WINCC) print2.c
