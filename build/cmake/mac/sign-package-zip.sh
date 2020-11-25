#!/bin/sh
cd $(dirname "$0")

codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}" --options runtime ./VookiImageViewer-brew-macos.zip
