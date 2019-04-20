#!/bin/sh

set -e

cd "$(dirname "$0")"
cd ..

echo "\n\n** Creating a changelog file - all tagged releases"
./debian/generate-changelog.sh

echo "\n\n** Creating a changelog file - latest tagged release to the head"
./debian/generate-changelog-snapshot.sh

CHANGELOG_VERSION=`head debian/changelog -n 1 | sed -e 's/^.\+(\([^)]\+\).\+/\1/' | sed -e 's/-1$//'`
echo "\n\n** Current version is $CHANGELOG_VERSION"

echo "\n\n** Creating an original package"
tar czvf ../vookiimageviewer_${CHANGELOG_VERSION}.orig.tar.gz ../vooki-image-viewer

echo "\n\n** Creating a DEB package"
dpkg-buildpackage -us -uc

echo "\n\n** Listing all resulting files"
ls -l ../

echo "\n\n** Resulting package information"
cd ..
dpkg --info `ls | grep '\.deb$' | grep -v 'dbgsym'`

echo "\n\n** Installing a resulting package"
dpkg -i `ls | grep '\.deb$' | grep -v 'dbgsym'`

echo "\n\n** Done"
