#------------------------
# FONTLIST.MAK make file
#------------------------

fontlist.exe : fontlist.obj fontlist.def fontlist.res
     $(WINLINK) fontlist, fontlist, NUL, $(WINLIB), fontlist
     rc -t fontlist.res

fontlist.obj : fontlist.c fontlist.h
     $(WINCC) fontlist.c

fontlist.res : fontlist.rc fontlist.h
     $(WINRC) fontlist.rc
