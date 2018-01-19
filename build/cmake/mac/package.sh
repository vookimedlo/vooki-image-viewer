#!/bin/sh

set -e;

VOLUME_NAME="VookiImageViewer"
DMG_NAME="VookiImageViewer.dmg"
DMG_TEMP_NAME="tmp-$DMG_NAME"
VOLUME_ICON_FILE="../../../src/resource/openclipart/vookiimageviewericon.icns"
BACKGROUND_FILE="support/vookiimageviewer-dmg-bg.png"

TARGET=package

echo "Removing old files"
rm -f "$DMG_NAME" || true
rm -f "$DMG_TEMP_NAME" || true
rm -rf "$TARGET" || true
mkdir "$TARGET" || true

echo "Creating disk image"
cp -rf ./build/VookiImageViewer.app "$TARGET/"
ln -s /Applications "$TARGET/Applications"
hdiutil create -volname "$VOLUME_NAME" -srcfolder "$TARGET/" -nocrossdev -ov -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW "$DMG_TEMP_NAME"

echo "Mounting disk image"
MOUNT_DIR="/Volumes/$VOLUME_NAME"
DEV_NAME=`hdiutil attach -readwrite -noverify -noautoopen $DMG_TEMP_NAME | egrep '^/dev/' | sed 1q | awk '{print $1}'`

echo "Setting background image"
test -d "$MOUNT_DIR/.background" || mkdir "$MOUNT_DIR/.background"
cp -f "$BACKGROUND_FILE" "$MOUNT_DIR/.background/background.png"

echo "Setting custom volume icon"
cp -f "$VOLUME_ICON_FILE" "$MOUNT_DIR/.VolumeIcon.icns"

echo "Executing script"
cat <<EOF | /usr/bin/osascript -l JavaScript
    var finder = Application("Finder");
    var disk = finder.disks["$VOLUME_NAME"];
    disk.open();
    var window = disk.containerWindow();
    window.currentView = "icon view";
    window.toolbarVisible = false;
    window.statusbarVisible = false;
    window.sidebarWidth = 135;
    window.bounds = {"x":30, "y":50, "width":550+135, "height":450};
    var options = window.iconViewOptions();
    options.iconSize = 64;
    options.arrangement = "not arranged";
    options.backgroundPicture = disk.files[".background:background.png"];

    disk.items[".VolumeIcon.icns"].position = {"x":1000, "y":1000};
    disk.items[".fseventsd"].position = {"x":1000, "y":1000};
    disk.items[".background"].position = {"x":1000, "y":1000};
    disk.items["VookiImageViewer.app"].position = {"x":280, "y":130};
    disk.items["Applications"].position = {"x":580, "y":130};

    disk.update({registeringApplications: false});
    delay(2);
    window.bounds = {"x":31, "y":50, "width":550+135, "height":450};
    window.bounds = {"x":30, "y":50, "width":550+135, "height":450};
    disk.update({registeringApplications: false});
    delay(2);

    disk.close();

    var dsStoreFile = disk.files[".DS_Store"];
    ObjC.import('Foundation');
    var fileManager = $.NSFileManager.defaultManager;

    while (!ObjC.unwrap(fileManager.fileExistsAtPath("$MOUNT_DIR/.DS_Store"))) {
        // give the finder time to write the .DS_Store file
        delay(1);
    }
EOF

echo "Setting custom volume icon"
cp -f "$VOLUME_ICON_FILE" "$MOUNT_DIR/.VolumeIcon.icns"
SetFile -c nC "$MOUNT_DIR/.VolumeIcon.icns"
SetFile -a C "$MOUNT_DIR"

echo "Fixing permissions"
chmod -Rf go-w "$MOUNT_DIR" || true

echo "Blessing image"
bless --folder "$MOUNT_DIR" --openfolder "$MOUNT_DIR"

echo "Unmounting disk image"
hdiutil detach "$DEV_NAME"

echo "Compressing disk image"
hdiutil convert "$DMG_TEMP_NAME" -format ULFO -o "$DMG_NAME"
rm -f "$DMG_TEMP_NAME"

echo "Done"

exit 0;
