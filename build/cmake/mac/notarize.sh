#!/bin/sh
cd $(dirname "$0")

xcrun altool -t osx -f VookiImageViewer.dmg --primary-bundle-id cz.VookiImageViewer --notarize-app --username $APPLE_ID_DEV
