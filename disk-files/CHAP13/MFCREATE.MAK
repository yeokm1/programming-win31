#------------------------
# MFCREATE.MAK make file
#------------------------

mfcreate.exe : mfcreate.obj mfcreate.def
     $(WINLINK) mfcreate, mfcreate, NUL, $(WINLIB), mfcreate
     rc -t mfcreate.exe

mfcreate.obj : mfcreate.c
     $(WINCC) mfcreate.c
