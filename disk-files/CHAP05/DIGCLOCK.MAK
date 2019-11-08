#------------------------
# DIGCLOCK.MAK make file
#------------------------

digclock.exe : digclock.obj digclock.def
     $(WINLINK) digclock, digclock, NUL, $(WINLIB), digclock
     rc -t digclock.exe

digclock.obj : digclock.c
     $(WINCC) digclock.c
