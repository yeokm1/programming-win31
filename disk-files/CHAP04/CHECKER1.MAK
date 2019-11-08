#------------------------
# CHECKER1.MAK make file
#------------------------

checker1.exe : checker1.obj checker1.def
     $(WINLINK) checker1, checker1, NUL, $(WINLIB), checker1
     rc -t checker1.exe

checker1.obj : checker1.c
     $(WINCC) checker1.c
