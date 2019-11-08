#------------------------
# CLIPVIEW.MAK make file
#------------------------

clipview.exe : clipview.obj clipview.def
     $(WINLINK) clipview, clipview, NUL, $(WINLIB), clipview
     rc -t clipview.exe

clipview.obj : clipview.c
     $(WINCC) clipview.c
