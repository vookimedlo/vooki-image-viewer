#!/bin/sh

target=VookiImageViewer.app/Contents/PlugIns/imageformats/

~/Qt/5.9.1/clang_64/bin/macdeployqt build/VookiImageViewer.app -always-overwrite

cd build
cp -f libvooki_kimg_pcx.so libvooki_kimg_pic.so libvooki_kimg_psd.so libvooki_kimg_ras.so libvooki_kimg_rgb.so libvooki_kimg_tga.so libvooki_kimg_xcf.so $target
cp -f  ../../../../src/plugins/kimageformats/psd.json ../../../../src/plugins/kimageformats/tga.json  ../../../../src/plugins/kimageformats/pcx.json ../../../../src/plugins/kimageformats/ras.json ../../../../src/plugins/kimageformats/xcf.json  ../../../../src/plugins/kimageformats/pic.json ../../../../src/plugins/kimageformats/rgb.json $target

cp -f libvooki_raw_thumb.so $target
cp -f  ../../../../src/plugins/rawthumb/rawthumb.json $target
