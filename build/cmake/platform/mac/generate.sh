#!/bin/sh
cd $(dirname "$0")/../../../../../vooki-image-viewer-build

cmake -DCMAKE_BUILD_TYPE=Release -Bbuild -S../vooki-image-viewer/
