#!/bin/sh
cd $(dirname "$0")/../../../../../vooki-image-viewer-build

cmake -DBUILD_DEPENDENCIES=OFF -DCMAKE_BUILD_TYPE=Release -Bbuild -S../vooki-image-viewer/
