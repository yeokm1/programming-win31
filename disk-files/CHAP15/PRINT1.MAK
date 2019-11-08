#----------------------
# PRINT1.MAK make file
#----------------------

print1.exe : print.obj print1.obj print1.def
     $(WINLINK) print1 print, print1, NUL, $(WINLIB), print1
     rc -t print1.exe

print.obj : print.c
     $(WINCC) print.c

print1.obj : print1.c
     $(WINCC) print1.c
