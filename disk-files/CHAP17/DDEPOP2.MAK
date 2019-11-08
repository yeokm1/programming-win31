#-----------------------
# DDEPOP2.MAK make file
#-----------------------

ddepop2.exe : ddepop2.obj ddepop2.def ddepop2.res
     $(WINLINK) ddepop2, ddepop2, NUL, $(WINLIB), ddepop2
     rc -t ddepop2.res

ddepop2.obj : ddepop2.c ddepop.h
     $(WINCC) ddepop2.c

ddepop2.res : ddepop2.rc ddepop.ico
     $(WINRC) ddepop2.rc
