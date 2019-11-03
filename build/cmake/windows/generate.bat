@echo off
cd /D "%~dp0"

if "%~1"=="no-qt-override" (
  echo Don't forget to set proper Qt env. variables
) else (
  echo Setting the Qt env. variables
  set QTDIR=c:\Qt\5.13.2\msvc2017_64
  set Qt5Core_DIR=%QTDIR%\lib\cmake\Qt5Core\
  set Qt5Gui_DIR=%QTDIR%\lib\cmake\Qt5Gui\
  set Qt5Widgets_DIR=%QTDIR%\lib\cmake\Qt5Widgets\
  set Qt5_DIR=%QTDIR%\lib\cmake\Qt5
)

cmake -G "Visual Studio 16 2019" -H. -Bbuild ..
cd /D "%~dp0"
