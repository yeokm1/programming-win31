#-----------------------
# BEEPER1.MAK make file
#-----------------------

beeper1.exe : beeper1.obj beeper1.def
     $(WINLINK) beeper1, beeper1, NUL, $(WINLIB), beeper1
     rc -t beeper1.exe

beeper1.obj : beeper1.c
     $(WINCC) beeper1.c
