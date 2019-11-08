#------------------------
# SCRAMBLE.MAK make file
#------------------------

scramble.exe : scramble.obj scramble.def
     $(WINLINK) scramble, scramble, NUL, $(WINLIB), scramble
     rc -t scramble.exe

scramble.obj : scramble.c
     $(WINCC) scramble.c
