#-----------------------
# DDEPOP1.MAK make file
#-----------------------

ddepop1.exe : ddepop1.obj ddepop1.def ddepop1.res
     $(WINLINK) ddepop1, ddepop1, NUL, $(WINLIB), ddepop1
     rc -t ddepop1.res

ddepop1.obj : ddepop1.c ddepop.h
     $(WINCC) ddepop1.c

ddepop1.res : ddepop1.rc ddepop.ico
     $(WINRC) ddepop1.rc
