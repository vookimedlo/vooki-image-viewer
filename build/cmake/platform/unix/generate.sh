#!/bin/sh
cd $(dirname "$0")

rm -rf build || true
cmake -DCMAKE_BUILD_TYPE=Release -H. -Bbuild ../../../..
