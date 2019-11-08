#------------------------
# CHECKER3.MAK make file
#------------------------

checker3.exe : checker3.obj checker3.def
     $(WINLINK) checker3, checker3, NUL, $(WINLIB), checker3
     rc -t checker3.exe

checker3.obj : checker3.c
     $(WINCC) checker3.c
