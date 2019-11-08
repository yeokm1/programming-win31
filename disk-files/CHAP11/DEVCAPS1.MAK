#------------------------
# DEVCAPS1.MAK make file
#------------------------

devcaps1.exe : devcaps1.obj devcaps.obj devcaps1.def devcaps1.res
     $(WINLINK) devcaps1 devcaps, devcaps1, NUL, $(WINLIB), devcaps1
     rc -t devcaps1.res

devcaps1.obj : devcaps1.c devcaps1.h
     $(WINCC) devcaps1.c

devcaps.obj : devcaps.c
     $(WINCC) devcaps.c

devcaps1.res : devcaps1.rc devcaps1.h
     $(WINRC) devcaps1.rc
