#-----------------------
# POPPAD1.MAK make file
#-----------------------

poppad1.exe : poppad1.obj poppad1.def
     $(WINLINK) poppad1, poppad1, NUL, $(WINLIB), poppad1.def
     rc -t poppad1.exe

poppad1.obj : poppad1.c
     $(WINCC) poppad1.c
