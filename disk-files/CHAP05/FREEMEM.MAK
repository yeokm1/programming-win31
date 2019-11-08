#-----------------------
# FREEMEM.MAK make file
#-----------------------

freemem.exe : freemem.obj freemem.def
     $(WINLINK) freemem, freemem, NUL, $(WINLIB), freemem
     rc -t freemem.exe

freemem.obj : freemem.c
     $(WINCC) freemem.c
