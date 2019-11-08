#------------------------
# RANDRECT.MAK make file
#------------------------

randrect.exe : randrect.obj randrect.def
     $(WINLINK) randrect, randrect, NUL, $(WINLIB), randrect
     rc -t randrect.exe

randrect.obj : randrect.c
     $(WINCC) randrect.c
