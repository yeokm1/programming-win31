#-----------------------
# COLORS3.MAK make file
#-----------------------

colors3.exe : colors3.obj colors3.def
     $(WINLINK) colors3, colors3, NUL, $(WINLIB), colors3
     rc -t colors3.exe

colors3.obj : colors3.c
     $(WINCC) colors3.c
