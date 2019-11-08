#-----------------------
# MDIDEMO.MAK make file
#-----------------------

mdidemo.exe : mdidemo.obj mdidemo.def mdidemo.res
     $(WINLINK) mdidemo, mdidemo, NUL, $(WINLIB), mdidemo
     rc -t mdidemo.res

mdidemo.obj : mdidemo.c mdidemo.h
     $(WINCC) mdidemo.c

mdidemo.res : mdidemo.rc mdidemo.h
     $(WINRC) mdidemo.rc
