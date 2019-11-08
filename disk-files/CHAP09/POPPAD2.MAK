#-----------------------
# POPPAD2.MAK make file
#-----------------------

poppad2.exe : poppad2.obj poppad2.def poppad2.res
     $(WINLINK) poppad2, poppad2, NUL, $(WINLIB), poppad2.def
     rc -t poppad2.res

poppad2.obj : poppad2.c poppad2.h
     $(WINCC) poppad2.c

poppad2.res : poppad2.rc poppad2.h poppad2.ico
     $(WINRC) poppad2.rc
