#!/bin/sh
cd $(dirname "$0")

xcrun stapler staple build/VookiImageViewer.App

# Check the DMG
#
# spctl -a -t open --context context:primary-signature -v VookiImageViewer.dmg

# Check the App
#
# spctl -a -v VookiImageViewer.app
