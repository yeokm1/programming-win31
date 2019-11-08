#-----------------------
# POEPOEM.MAK make file
#-----------------------

poepoem.exe : poepoem.obj poepoem.def poepoem.res
     $(WINLINK) poepoem, poepoem, NUL, $(WINLIB), poepoem
     rc -t poepoem.res

poepoem.obj : poepoem.c poepoem.h
     $(WINCC) poepoem.c

poepoem.res : poepoem.rc poepoem.ico poepoem.asc poepoem.h
     $(WINRC) poepoem.rc
