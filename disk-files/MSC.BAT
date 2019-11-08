REM -------------------------------------------------------------
REM  MSC.BAT -- Set up environment for Microsoft C/C++ 7.0 NMAKE
REM -------------------------------------------------------------
SET WINCC=cl -c -G2sw -Ow -W3 -Zp -Tp
SET WINLINK=link /nod
SET WINLIB=slibcew oldnames libw commdlg ddeml
SET WINRC=rc -r
