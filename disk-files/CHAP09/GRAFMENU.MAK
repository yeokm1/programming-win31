#------------------------
# GRAFMENU.MAK make file
#------------------------

grafmenu.exe : grafmenu.obj grafmenu.def grafmenu.res
     $(WINLINK) grafmenu, grafmenu, NUL, $(WINLIB), grafmenu
     rc -t -31 grafmenu.res

grafmenu.obj : grafmenu.c grafmenu.h
     $(WINCC) grafmenu.c

grafmenu.res : grafmenu.rc grafmenu.h \
               editlabl.bmp filelabl.bmp fontlabl.bmp bighelp.bmp
     $(WINRC) grafmenu.rc
