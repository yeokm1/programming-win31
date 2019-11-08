#----------------------
# ABOUT3.MAK make file
#----------------------

about3.exe : about3.obj about3.def about3.res
     $(WINLINK) about3, about3, NUL, $(WINLIB), about3
     rc -t about3.res

about3.obj : about3.c about3.h
     $(WINCC) about3.c

about3.res : about3.rc about3.h about3.ico
     $(WINRC) about3.rc
