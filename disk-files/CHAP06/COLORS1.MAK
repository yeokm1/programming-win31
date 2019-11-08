#-----------------------
# COLORS1.MAK make file
#-----------------------

colors1.exe : colors1.obj colors1.def
     $(WINLINK) colors1, colors1, NUL, $(WINLIB), colors1
     rc -t colors1.exe

colors1.obj : colors1.c
     $(WINCC) colors1.c
