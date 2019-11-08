#------------------------
# ROP2LOOK.MAK make file
#------------------------

rop2look.exe : rop2look.obj rop2look.def rop2look.res
     $(WINLINK) rop2look, rop2look, NUL, $(WINLIB), rop2look
     rc -t rop2look.res

rop2look.obj : rop2look.c
     $(WINCC) rop2look.c

rop2look.res : rop2look.rc
     $(WINRC) rop2look.rc
