@echo off
mkdir build
cd build


set QTDIR=c:\Qt\5.12.2\msvc2017_64
set Qt5Core_DIR=%QTDIR%\lib\cmake\Qt5Core\
set Qt5Gui_DIR=%QTDIR%\lib\cmake\Qt5Gui\
set Qt5Widgets_DIR=%QTDIR%\lib\cmake\Qt5Widgets\
set Qt5_DIR=%QTDIR%\lib\cmake\Qt5

cmake -G "Visual Studio 15 2017 Win64" ../../
