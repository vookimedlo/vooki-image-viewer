#!/bin/sh
cd $(dirname "$0")/../../../../../vooki-image-viewer-build

cd build/build/cmake

find VookiImageViewer.app -name "*.dylib" | xargs codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}"
find VookiImageViewer.app -name "*.so" | xargs codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}"
find VookiImageViewer.app -name "Qt*" -type f | xargs codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}"

codesign --force --verify --verbose --sign "${DEVELOPER_IDENTITY_VOOKIIMAGEVIEWER}" --options runtime ./VookiImageViewer.app
