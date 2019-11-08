REM --------------------------------------------------------
REM  BCP.BAT -- Set up environment for Borland C++ 3.1 MAKE
REM --------------------------------------------------------
SET WINCC=bcc -c -w-par -P -W -2
SET WINLINK=tlink /c /n /Tw /L\borlandc\lib c0ws
SET WINLIB=import mathws cws
SET WINRC=rc -r -i\borlandc\include
