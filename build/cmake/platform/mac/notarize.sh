#!/bin/sh
cd $(dirname "$0")

xcrun notarytool submit --wait --keychain-profile "AC_PRIVATE" VookiImageViewer.dmg

# Add a ticket so Gatekeeper can find the ticket even when a network connection isn’t available on user's machine
#
xcrun stapler staple VookiImageViewer.dmg

# Check the DMG
#
spctl -a -t open --context context:primary-signature -v VookiImageViewer.dmg