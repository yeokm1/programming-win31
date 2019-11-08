#-----------------------
# BLOWUP2.MAK make file
#-----------------------

blowup2.exe : blowup2.obj blowup2.def
     $(WINLINK) blowup2, blowup2, NUL, $(WINLIB), blowup2
     rc -t blowup2.exe

blowup2.obj : blowup2.c
     $(WINCC) blowup2.c
