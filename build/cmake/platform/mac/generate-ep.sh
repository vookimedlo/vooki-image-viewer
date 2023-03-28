#!/bin/sh
cd $(dirname "$0")/../../../../../vooki-image-viewer-build

rm -rf build || true
cmake -DBUILD_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Release -Bbuild -S../vooki-image-viewer/
