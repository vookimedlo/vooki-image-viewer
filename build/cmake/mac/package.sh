#!/bin/sh

target=package
rm -rf "$target"
mkdir "$target"
cp -fr ./build/VookiImageViewer.app "$target/"
ln -s /Applications "$target/Applications"
hdiutil create -volname 'VookiImageViewer [https://VookiImageViewer.cz]' -srcfolder "$target/" -ov -format ULFO VookiImageViewer.dmg
