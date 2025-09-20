@echo off
cd /D "%~dp0"

if "%~1"=="no-qt-override" (
  echo Don't forget to set proper Qt env. variables
) else (
  echo Setting the Qt env. variables
  set "QTDIR=c:\Qt\6.8.0\msvc2022_64"
)

cmake -A x64 -G "Visual Studio 17 2022" -H. -Bbuild ../../../.. -Wno-dev -DQt6_DIR="%QTDIR%\lib\cmake\Qt6" -DQt6CoreTools_DIR="%QTDIR%\lib\cmake\Qt6Core" -DQt6CoreTools_DIR="%QTDIR%\lib\cmake\Qt6CoreTools" -DQt6Gui_DIR="%QTDIR%\lib\cmake\Qt6Gui" -DQt6GuiTools_DIR="%QTDIR%\lib\cmake\Qt6GuiTools" -DQt6Widgets_DIR="%QTDIR%\lib\cmake\Qt6Widgets" -DQt6WidgetsTools_DIR="%QTDIR%\lib\cmake\Qt6WidgetsTools"
cd /D "%~dp0"
