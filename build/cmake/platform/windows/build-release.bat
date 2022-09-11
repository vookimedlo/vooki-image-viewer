@echo off
cd /D "%~dp0"

cmake --build build --config Release --target package

cd /D "%~dp0"

