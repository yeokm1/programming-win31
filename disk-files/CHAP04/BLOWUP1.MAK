#-----------------------
# BLOWUP1.MAK make file
#-----------------------

blowup1.exe : blowup1.obj blowup1.def
     $(WINLINK) blowup1, blowup1, NUL, $(WINLIB), blowup1
     rc -t blowup1.exe

blowup1.obj : blowup1.c
     $(WINCC) blowup1.c
