#----------------------
# ABOUT2.MAK make file
#----------------------

about2.exe : about2.obj about2.def about2.res
     $(WINLINK) about2, about2, NUL, $(WINLIB), about2
     rc -t about2.res

about2.obj : about2.c about2.h
     $(WINCC) about2.c

about2.res : about2.rc about2.h about2.ico
     $(WINRC) about2.rc
