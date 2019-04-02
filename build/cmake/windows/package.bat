@echo off

cd /D "%~dp0"

(robocopy build\Release\ "support\package\SourceDir\PFiles\Michal Duda\VookiImageViewer" * /MIR)
IF %%ERRORLEVEL%% GEQ 4 EXIT /b %%ERRORLEVEL%%

if "%~1"=="release" (
SET name=VookiImageViewer-x64-win10.msi
) else (
SET name=VookiImageViewer-x64-win10-TESTING_BUILD.msi
)

cd support\package\
"%WIX%\bin\candle.exe" -arch x64 .\package.xml
"%WIX%\bin\light.exe" -ext WixUIExtension  .\package.wixobj -out %NAME%
