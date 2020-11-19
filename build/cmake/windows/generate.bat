@echo off
cd /D "%~dp0"

if "%~1"=="no-qt-override" (
  echo Don't forget to set proper Qt env. variables
) else (
  echo Setting the Qt env. variables
  set "QTDIR=c:\Qt\5.15.1\msvc2019_64"
)

cmake -A x64 -G "Visual Studio 16 2019" -H. -Bbuild .. -DQt5_DIR="%QTDIR%\lib\cmake\Qt5"
cd /D "%~dp0"
