#----------------------
# POPPAD.MAK make file
#----------------------

poppad.exe : poppad.obj  popfile.obj popfind.obj \
             popfont.obj popprnt.obj poppad.res  poppad.def
     $(WINLINK) poppad popfile popfind popfont popprnt, poppad , \
          NUL, $(WINLIB), poppad
     rc -t poppad.res poppad.exe

poppad.obj : poppad.c poppad.h
     $(WINCC) poppad.c

popfile.obj : popfile.c
     $(WINCC) popfile.c

popfind.obj : popfind.c
     $(WINCC) popfind.c

popfont.obj : popfont.c
     $(WINCC) popfont.c

popprnt.obj : popprnt.c
     $(WINCC) popprnt.c

poppad.res : poppad.rc poppad.h poppad.ico
     $(WINRC) poppad.rc
