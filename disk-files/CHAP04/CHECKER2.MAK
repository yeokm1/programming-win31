#------------------------
# CHECKER2.MAK make file
#------------------------

checker2.exe : checker2.obj checker2.def
     $(WINLINK) checker2, checker2, NUL, $(WINLIB), checker2
     rc -t checker2.exe

checker2.obj : checker2.c
     $(WINCC) checker2.c
