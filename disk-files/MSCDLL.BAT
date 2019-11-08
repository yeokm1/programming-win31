REM ------------------------------------------------------------
REM  MSCDLL.BAT -- Set up environment for Microsoft C 7.0 NMAKE
REM ------------------------------------------------------------
SET DLLCC=cl -c -ASw -G2sw -Ow -W3 -Zp -Tp
SET DLLLINK=link /nod libentry
SET DLLLIB=sdllcew oldnames libw
SET DLLRC=rc -r
