@echo off
cd /D "%~dp0"

cmake --build build --config Release

cd build
cpack -C Release --config ./CPackConfig.cmake

cd /D "%~dp0"

