@echo off
cd /D "%~dp0"

%QTDIR%\bin\windeployqt.exe --release --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw build\Release\VookiImageViewer.exe

FOR /F "tokens=* USEBACKQ" %%F IN (`%QTDIR%\bin\qtpaths.exe --qt-version`) DO (
SET QT_VERSION_DEV=%%F
)

(robocopy ..\prebuilt\qtimageformats\windows\%QT_VERSION_DEV%\ build\Release\imageformats\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy build\Release\ build\Release\imageformats\ vooki_*.dll /MOV /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibHEIF-1.9.1\lib\ build\Release\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

(robocopy 3rdPartyLibs\LibDe265-1.0.8\lib\ build\Release\ *.dll /IS /IT)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

EXIT /B 0
