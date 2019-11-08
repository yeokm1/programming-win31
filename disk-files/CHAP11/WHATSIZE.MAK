#------------------------
# WHATSIZE.MAK make file
#------------------------

whatsize.exe : whatsize.obj whatsize.def
     $(WINLINK) whatsize, whatsize, NUL, $(WINLIB), whatsize
     rc -t whatsize.exe

whatsize.obj : whatsize.c
     $(WINCC) whatsize.c
