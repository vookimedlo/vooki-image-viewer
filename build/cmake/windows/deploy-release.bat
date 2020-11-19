@echo off
cd /D "%~dp0"

%QTDIR%\bin\windeployqt.exe --release --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-webkit2 --no-opengl-sw --no-angle build\Release\VookiImageViewer.exe

(robocopy build\Release\ build\Release\imageformats\ vooki_*.dll /MOV /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibHEIF-1.9.1\lib\ build\Release\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibDe265-1.0.8\lib\ build\Release\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

EXIT /B 0
