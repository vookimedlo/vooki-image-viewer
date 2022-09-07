#!/bin/sh
cd $(dirname "$0")

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ../../../
