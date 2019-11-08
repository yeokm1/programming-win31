#-----------------------
# KEYLOOK.MAK make file
#-----------------------

keylook.exe : keylook.obj keylook.def
     $(WINLINK) keylook, keylook, NUL, $(WINLIB), keylook
     rc -t keylook.exe

keylook.obj : keylook.c
     $(WINCC) keylook.c
