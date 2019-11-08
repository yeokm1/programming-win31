#------------------------
# PICKFONT.MAK make file
#------------------------

pickfont.exe : pickfont.obj pickfont.def pickfont.res
     $(WINLINK) pickfont, pickfont, NUL, $(WINLIB), pickfont
     rc -t pickfont.res

pickfont.obj : pickfont.c pickfont.h
     $(WINCC) pickfont.c

pickfont.res : pickfont.rc pickfont.h
     $(WINRC) pickfont.rc
