@rem Usage: wxg2xrc.bat destdir
@rem <Converts dialog files from .wxg to .xrc format

setlocal

set DialogTemp=%temp%\wx_catapult
md %DialogTemp%

pushd ..\..\dialogs
for %%f in (*.wxg) do ..\sed.exe -f ..\build\wxg2xrc.sed %%f > %DialogTemp%\%%f
popd

set OutDir=%1
if exist %OutDir% (rd /s /q %OutDir%)
md %OutDir%
move /y %DialogTemp%\*.wxg %OutDir%
ren %OutDir%\*.wxg *.xrc

rd /s /q %DialogTemp%

endlocal
