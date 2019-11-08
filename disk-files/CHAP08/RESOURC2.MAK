#------------------------
# RESOURC2.MAK make file
#------------------------

resourc2.exe : resourc2.obj resourc2.def resourc2.res
     $(WINLINK) resourc2, resourc2, NUL, $(WINLIB), resourc2
     rc -t resourc2.res

resourc2.obj : resourc2.c
     $(WINCC) resourc2.c

resourc2.res : resourc2.rc resourc2.ico resourc2.cur resourc2.bmp
     $(WINRC) resourc2.rc
