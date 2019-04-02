@echo off
cd /D "%~dp0"

%QTDIR%\bin\windeployqt.exe --release --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-webkit2 --no-opengl-sw --no-angle build\Release\VookiImageViewer.exe

robocopy build\Release\ build\Release\imageformats\ vooki_*.dll /MOV /IS /IT
robocopy 3rdPartyLibs\LibHEIF-1.4.0\lib\ build\Release\ *.dll /IS /IT
robocopy 3rdPartyLibs\LibDe265-1.0.3\lib\ build\Release\ *.dll /IS /IT
