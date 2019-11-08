#------------------------
# FORMFEED.MAK make file
#------------------------

formfeed.exe: formfeed.obj formfeed.def
     $(WINLINK) formfeed, formfeed, NUL, $(WINLIB), formfeed
     rc -t formfeed.exe

formfeed.obj: formfeed.c
     $(WINCC) formfeed.c
