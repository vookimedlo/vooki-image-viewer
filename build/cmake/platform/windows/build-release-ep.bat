@echo off
cd /D "%~dp0"

cmake --build build --config Release

cd /D "%~dp0"
