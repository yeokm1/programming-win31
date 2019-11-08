#-----------------------
# HEXCALC.MAK make file
#-----------------------

hexcalc.exe: hexcalc.obj hexcalc.def hexcalc.res
     $(WINLINK) hexcalc, hexcalc, NUL, $(WINLIB), hexcalc
     rc -t hexcalc.res

hexcalc.obj: hexcalc.c
     $(WINCC) hexcalc.c

hexcalc.res : hexcalc.rc hexcalc.ico
     $(WINRC) hexcalc.rc
