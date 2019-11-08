#--------------------
# ARCS.MAK make file
#--------------------

arcs.exe : arcs.obj arcs.def arcs.res
     $(WINLINK) arcs, arcs, NUL, $(WINLIB), arcs
     rc -t arcs.res

arcs.obj : arcs.c arcs.h
     $(WINCC) arcs.c

arcs.res : arcs.rc arcs.h
     $(WINRC) arcs.rc
