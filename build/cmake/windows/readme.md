Windows Build Environment
=========================

Expected tools
--------------

-   Qt
-   CMake
-   Visual Studio 2017 Community Edition

Expected environment variables
------------------------------

| Environment Variable  | Value                         |
|-----------------------|-------------------------------|
| QTDIR                 | c:\Qt\5.12.1\msvc2017_64       |
| Qt5Core_DIR           | %QTDIR%\lib\cmake\Qt5Core\    |
| Qt5Gui_DIR            | %QTDIR%\lib\cmake\Qt5Gui\     |
| Qt5Widgets_DIR        | %QTDIR%\lib\cmake\Qt5Widgets\ |

Visual Studio 2017 Win64 project generation
-------------------------------------------

Run the generate.bat.

Deployment
----------

Run the deploy.bat to copy all dependant files to the resulting build directory _build/Release.
