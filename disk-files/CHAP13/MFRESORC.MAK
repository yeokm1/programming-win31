#------------------------
# MFRESORC.MAK make file
#------------------------

mfresorc.exe : mfresorc.obj mfresorc.def mfresorc.res
     $(WINLINK) mfresorc, mfresorc, NUL, $(WINLIB), mfresorc
     rc -t mfresorc.res

mfresorc.obj : mfresorc.c
     $(WINCC) mfresorc.c

mfresorc.res : mfresorc.rc mylogo.wmf
     $(WINRC) mfresorc.rc
