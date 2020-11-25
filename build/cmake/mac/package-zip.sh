#!/bin/sh
cd $(dirname "$0")

set -e

LANG="C"

PLIST_FILE=build/VookiImageViewer.app/Contents/Info.plist

VERSION=$(/usr/libexec/PlistBuddy -c "Print CFBundleVersion" $PLIST_FILE)
SHORT_VERSION_STRING=$(/usr/libexec/PlistBuddy -c "Print CFBundleShortVersionString" $PLIST_FILE)

ZIP_NAME="VookiImageViewer-brew-macos.zip"
ZIP_PATH="./$ZIP_NAME"

rm -f "$ZIP_PATH" || true
ditto -c -k --sequesterRsrc --keepParent build/VookiImageViewer.app $ZIP_PATH
echo "$ZIP_NAME was created."
SHASUM=$(shasum -ba 256 $ZIP_PATH)
SHA=$(echo $SHASUM | cut -f 1 -d\ )
echo "$SHASUM"

CASK_PATH="$RELEASE_OUTPUT_DIR/ck550-macos.rb"

cat > "./vookiimageviewer-macos.rb" <<EOF
cask 'vookiimageviewer-macos' do
version '$SHORT_VERSION_STRING'
url 'https://github.com/vookimedlo/vooki-image-viewer/releases/download/v$SHORT_VERSION_STRING/VookiImageViewer-brew-macos.zip'
sha256 '$SHA'

name 'VookiImageViewer'
homepage "https://github.com/vookimedlo/vooki-image-viewer"

app 'VookiImageViewer.app'
end
EOF

echo "Homebrew CASK file was created."
