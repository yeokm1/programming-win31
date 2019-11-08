#------------------------
# NOPOPUPS.MAK make file
#------------------------

nopopups.exe : nopopups.obj nopopups.def nopopups.res
     $(WINLINK) nopopups, nopopups, NUL, $(WINLIB), nopopups
     rc -t nopopups.res

nopopups.obj : nopopups.c nopopups.h
     $(WINCC) nopopups.c

nopopups.res : nopopups.rc nopopups.h
     $(WINRC) nopopups.rc
