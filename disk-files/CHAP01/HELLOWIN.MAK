#------------------------
# HELLOWIN.MAK make file
#------------------------

hellowin.exe : hellowin.obj hellowin.def
     $(WINLINK) hellowin, hellowin, NUL, $(WINLIB), hellowin
     rc -t hellowin.exe

hellowin.obj : hellowin.c
     $(WINCC) hellowin.c
