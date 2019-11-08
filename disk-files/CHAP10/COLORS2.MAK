#-----------------------
# COLORS2.MAK make file
#-----------------------

colors2.exe : colors2.obj colors2.def colors2.res
     $(WINLINK) colors2, colors2, NUL, $(WINLIB), colors2
     rc -t colors2.res

colors2.obj : colors2.c
     $(WINCC) colors2.c

colors2.res : colors2.rc
     $(WINRC) colors2.rc
