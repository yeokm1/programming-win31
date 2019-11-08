#-----------------------
# POPPAD3.MAK make file
#-----------------------

poppad3.exe : poppad.obj   popfile.obj  popfind.obj \
              popfont0.obj popprnt0.obj poppad.res  poppad.def
     $(WINLINK) poppad popfile popfind popfont0 popprnt0, poppad3, \
          NUL, $(WINLIB), poppad
     rc -t poppad.res poppad3.exe

poppad.obj : poppad.c poppad.h
     $(WINCC) poppad.c

popfile.obj : popfile.c
     $(WINCC) popfile.c

popfind.obj : popfind.c
     $(WINCC) popfind.c

popfont0.obj : popfont0.c
     $(WINCC) popfont0.c

popprnt0.obj : popprnt0.c
     $(WINCC) popprnt0.c

poppad.res : poppad.rc poppad.h poppad.ico
     $(WINRC) poppad.rc
