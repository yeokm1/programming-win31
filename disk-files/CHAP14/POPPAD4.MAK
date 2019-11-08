#-----------------------
# POPPAD4.MAK make file
#-----------------------

poppad4.exe : poppad.obj  popfile.obj  popfind.obj \
              popfont.obj popprnt0.obj poppad.res  poppad.def
     $(WINLINK) poppad popfile popfind popfont popprnt0, poppad4, \
          NUL, $(WINLIB), poppad
     rc -t poppad.res poppad4.exe

poppad.obj : poppad.c poppad.h
     $(WINCC) poppad.c

popfile.obj : popfile.c
     $(WINCC) popfile.c

popfind.obj : popfind.c
     $(WINCC) popfind.c

popfont.obj : popfont.c
     $(WINCC) popfont.c

popprnt0.obj : popprnt0.c
     $(WINCC) popprnt0.c

poppad.res : poppad.rc poppad.h poppad.ico
     $(WINRC) poppad.rc
