#!/bin/sh
cd $(dirname "$0")

cmake --build build --config Release --target package
