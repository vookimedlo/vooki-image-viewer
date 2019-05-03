#!/bin/sh

set -e

cd $(dirname "$0")
readonly SCRIPT_LOCATION=`pwd`

readonly SNAP_GIT_RELEASE=`git rev-parse --short HEAD`
readonly SNAP_DATE_RELEASE=`date +%Y%m%d`
readonly SNAP_RELEASE="${SNAP_DATE_RELEASE}git${SNAP_GIT_RELEASE}"
echo -e  "\n\n** Snapshot version - ${SNAP_RELEASE}"

echo -e  "\n\n** Patching a Release field in a spec file"
sed "s/^Release: .\+$/Release: ${SNAP_RELEASE}%{?dist}/" vookiimageviewer.spec > vookiimageviewer-patched.spec

echo -e  "\n\n** Creating RPM build structure for current user"
rpmdev-setuptree

echo -e  "\n\n** Creating a changelog file - all tagged releases"
#./debian/generate-changelog.sh

echo -e  "\n\n** Creating a changelog file - latest tagged release to the head"
#./debian/generate-changelog-snapshot.sh

#CHANGELOG_VERSION=`head debian/changelog -n 1 | sed -e 's/^.\+(\([^)]\+\).\+/\1/' | sed -e 's/-1$//'`
#echo -e  "\n\n** Current version is $CHANGELOG_VERSION"

CHANGELOG_VERSION=`grep 'Version:' vookiimageviewer-patched.spec | cut -d' ' -f2`
cd ../../../../../../../

echo -e  "\n\n** Creating an original package"
tar cjvf vookiimageviewer-${CHANGELOG_VERSION}.tar.bz2 vooki-image-viewer
cp -f vookiimageviewer-${CHANGELOG_VERSION}.tar.bz2 ~/rpmbuild/SOURCES/

echo -e  "\n\n** Creating a RPM package"
cd "${SCRIPT_LOCATION}"
rpmbuild -ba vookiimageviewer-patched.spec

echo -e  "\n\n** Listing all resulting files"
ls -l ~/rpmbuild/SRPMS/
ls -Rl ~/rpmbuild/RPMS/

echo -e  "\n\n** Resulting packages information"
find ~/rpmbuild/ -type f -name "*.rpm" | xargs -n1 rpm -qi || true

echo -e  "\n\n** Installing a resulting package"
find ~/rpmbuild/ -type f -name "*.rpm" | grep -v debug | grep -v '.src.' | xargs rpm -ivh || true

echo -e  "\n\n** Removing installed package"
rpm -e vookiimageviewer || true

echo -e  "\n\n** Done"
