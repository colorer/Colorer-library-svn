@echo off
rem This file is for calling colorer as a filter.
rem stdin - input text, stdout - result text
rem it's good for using in FAR editor :)

more > %TMP%\clr.tmp
echo ^<!--
type %TMP%\clr.tmp
echo --^>
echo.
colorer -dc -dh -h %TMP%\clr.tmp %1 %2 %3 %4 %5 %6
del %TMP%\clr.tmp
