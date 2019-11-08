#-----------------------
# DEVCAPS.MAK make file
#-----------------------

devcaps2.exe : devcaps2.obj devcaps.obj devcaps2.res devcaps2.def
     $(WINLINK) devcaps2 devcaps, devcaps2, NUL, $(WINLIB), devcaps2
     rc -t devcaps2.res

devcaps2.obj : devcaps2.c devcaps2.h
     $(WINCC) devcaps2.c

devcaps.obj : devcaps.c
     $(WINCC) devcaps.c

devcaps2.res : devcaps2.rc devcaps2.h
     $(WINRC) devcaps2.rc
