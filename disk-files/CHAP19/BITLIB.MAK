#----------------------
# BITLIB.MAK make file
#----------------------

bitlib.dll : bitlib.obj bitlib.def bitlib.res
     $(DLLLINK) bitlib, bitlib.dll, NUL, $(DLLLIB), bitlib
     rc bitlib.res bitlib.dll

bitlib.obj : bitlib.c
     $(DLLCC) bitlib.c

bitlib.res : bitlib.rc
     $(DLLRC) bitlib.rc
