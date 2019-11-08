#------------------------
# RESOURC1.MAK make file
#------------------------

resourc1.exe : resourc1.obj resourc1.def resourc1.res
     $(WINLINK) resourc1, resourc1, NUL, $(WINLIB), resourc1
     rc -t resourc1.res

resourc1.obj : resourc1.c
     $(WINCC) resourc1.c

resourc1.res : resourc1.rc resourc1.ico resourc1.cur
     $(WINRC) resourc1.rc
