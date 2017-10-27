@echo off

robocopy build\Release\ "support\package\SourceDir\PFiles\Michal Duda\VookiImageViewer" * /MIR

cd support\package\
"C:\Program Files (x86)\WiX Toolset v3.11\bin\candle.exe" -arch x64 .\package.xml
"C:\Program Files (x86)\WiX Toolset v3.11\bin\light.exe" -ext WixUIExtension  .\package.wixobj -out VookiImageViewer-x64-win10.msi
