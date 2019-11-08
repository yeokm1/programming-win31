#----------------------
# CLOVER.MAK make file
#----------------------

clover.exe : clover.obj clover.def
     $(WINLINK) clover, clover, NUL, $(WINLIB), clover
     rc -t clover.exe

clover.obj : clover.c
     $(WINCC) clover.c
