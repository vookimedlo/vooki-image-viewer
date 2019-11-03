#!/bin/sh
cd $(dirname "$0")

target=VookiImageViewer.app/Contents/PlugIns/imageformats/

~/Qt/${QT_VERSION_DEV}/clang_64/bin/macdeployqt build/VookiImageViewer.app -always-overwrite

cd build
cp -f libvooki_kimg_hdr.so libvooki_kimg_pcx.so libvooki_kimg_pic.so libvooki_kimg_psd.so libvooki_kimg_ras.so libvooki_kimg_rgb.so libvooki_kimg_tga.so libvooki_kimg_xcf.so $target
cp -f libvooki_raw_thumb.so $target
