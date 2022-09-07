#!/bin/sh
cd $(dirname "$0")

rm -rf build || true

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../../../
