#-----------------------
# BTNLOOK.MAK make file
#-----------------------

btnlook.exe : btnlook.obj btnlook.def
     $(WINLINK) btnlook, btnlook, NUL, $(WINLIB), btnlook
     rc -t btnlook.exe

btnlook.obj : btnlook.c
     $(WINCC) btnlook.c
