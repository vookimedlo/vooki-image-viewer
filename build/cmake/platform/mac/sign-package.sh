#!/bin/sh
cd $(dirname "$0")/../../../../../vooki-image-viewer-build

codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}" --options runtime ./VookiImageViewer.dmg
