#----------------------
# ABOUT1.MAK make file
#----------------------

about1.exe : about1.obj about1.def about1.res
     $(WINLINK) about1, about1, NUL, $(WINLIB), about1
     rc -t about1.res

about1.obj : about1.c about1.h
     $(WINCC) about1.c

about1.res : about1.rc about1.h about1.ico
     $(WINRC) about1.rc
