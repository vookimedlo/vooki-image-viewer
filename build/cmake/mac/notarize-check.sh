#!/bin/sh
cd $(dirname "$0")

xcrun altool --username apple@vookimedlo.cz --notarization-info "$1"
