#!/bin/sh
cd $(dirname "$0")

echo "\n == Generating makefiles ..."
./generate.sh

echo "\n == Building ..."
./build.sh

echo "\n == Signing binaries ..."
./sign.sh

echo "\n == Creating DMG ..."
./package.sh

echo "\n == Signing DMG ..."
./sign-package.sh

echo "\n == Notarizing and stapling ..."
./notarize.sh

echo "\n == Creating ZIP ..."
./package-zip.sh

echo "\n == Signing ZIP ..."
./sign-package-zip.sh

echo "\n == Stapling app ..."
./staple-zip.sh

echo "\n == Creating ZIP with a ticket ..."
./package-zip.sh