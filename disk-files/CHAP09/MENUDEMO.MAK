#------------------------
# MENUDEMO.MAK make file
#------------------------

menudemo.exe : menudemo.obj menudemo.def menudemo.res
     $(WINLINK) menudemo, menudemo, NUL, $(WINLIB), menudemo
     rc -t menudemo.res

menudemo.obj : menudemo.c menudemo.h
     $(WINCC) menudemo.c

menudemo.res : menudemo.rc menudemo.h
     $(WINRC) menudemo.rc
