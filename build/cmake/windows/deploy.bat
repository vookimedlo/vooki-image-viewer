@echo off
%QTDIR%\bin\windeployqt.exe --release --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-webkit2 --no-opengl-sw --no-angle build\Release\VookiImageViewer.exe

robocopy build\Release\ build\Release\imageformats\ vooki_*.dll /MOV /IS /IT
