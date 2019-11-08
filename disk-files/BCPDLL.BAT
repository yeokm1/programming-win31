REM -----------------------------------------------------------
REM  BCPDLL.BAT -- Set up environment for Borland C++ 3.0 MAKE
REM -----------------------------------------------------------
SET DLLCC=bcc -c -ms! -w-par -P -W -2
SET DLLLINK=tlink /c /n /Tw /L\borlandc\lib c0ds
SET DLLLIB=import mathws cws
SET DLLRC=rc -r -i\borlandc\include
