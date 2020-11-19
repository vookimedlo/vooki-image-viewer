@echo off
cd /D "%~dp0"

%QTDIR%\bin\windeployqt.exe --debug --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-webkit2 --no-opengl-sw --no-angle build\Debug\VookiImageViewer.exe

(robocopy build\Debug\ build\Debug\imageformats\ vooki_*.dll /MOV /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibHEIF-1.9.1\lib\ build\Debug\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibDe265-1.0.8\lib\ build\Debug\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

EXIT /B 0
