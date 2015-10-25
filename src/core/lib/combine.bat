@echo off 

set path=E:\ide\VC6.0.G\VC98\Bin;E:\ide\VC6.0.G\Common\MSDev98\Bin
if exist "%1" del /F "%1"
lib /NOLOGO /out:"%1" "%2"
if exist "%1" goto OK
echo Fail to build %1
goto end
:OK
echo Generate %1 OK.
:end
exit 