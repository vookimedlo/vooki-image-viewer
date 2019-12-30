Windows Build Environment
=========================

Expected tools
--------------

-   Qt
-   CMake
-   Visual Studio 2019 Community Edition

Expected environment variables
------------------------------

| Environment Variable  | Value                         |
|-----------------------|-------------------------------|
| QTDIR                 | c:\Qt\5.14.0\msvc2017_64      |
| Qt5Core_DIR           | %QTDIR%\lib\cmake\Qt5Core\    |
| Qt5Gui_DIR            | %QTDIR%\lib\cmake\Qt5Gui\     |
| Qt5Widgets_DIR        | %QTDIR%\lib\cmake\Qt5Widgets\ |
| Qt5_DIR               | %QTDIR%\lib\cmake\Qt5         |

Visual Studio 2019 x64 project generation
-------------------------------------------

Run the generate.bat.

Deployment
----------

Run the deploy.bat to copy all dependant files to the resulting build directory _build/Release._
