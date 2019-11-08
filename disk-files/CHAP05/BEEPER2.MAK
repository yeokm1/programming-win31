#-----------------------
# BEEPER2.MAK make file
#-----------------------

beeper2.exe : beeper2.obj beeper2.def
     $(WINLINK) beeper2, beeper2, NUL, $(WINLIB), beeper2
     rc -t beeper2.exe

beeper2.obj : beeper2.c
     $(WINCC) beeper2.c
