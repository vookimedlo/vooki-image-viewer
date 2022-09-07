#!/bin/sh
cd $(dirname "$0")

set -e

readonly SCRIPT_LOCATION=`pwd`

cp -f changelog/git2changelog/scripts/git2changelog .
cp -f changelog/git2changelog/src/git2changelog.py .

readonly SNAP_GIT_RELEASE=`git rev-parse --short HEAD`
readonly SNAP_DATE_RELEASE=`date +%Y%m%d`
readonly SNAP_RELEASE="${SNAP_DATE_RELEASE}git${SNAP_GIT_RELEASE}"
echo -e  "\n\n** Snapshot version - ${SNAP_RELEASE}"

echo -e  "\n\n** Patching a Release field in a spec file"
sed "s/^Release: .\+$/Release: ${SNAP_RELEASE}%{?dist}/" vookiimageviewer.spec > vookiimageviewer-patched.spec

echo -e  "\n\n** Creating RPM build structure for current user"
rpmdev-setuptree

echo -e  "\n\n** Fetching git tags"
git fetch -t
git tag -n

echo -e  "\n\n** Creating a changelog file - all tagged releases up to the HEAD"
echo -e  "%changelog" >> vookiimageviewer-patched.spec
python ./git2changelog -b v2017.10.27 -r ../../../../../../.. >> vookiimageviewer-patched.spec

echo -e  "\n\n** Creating a changelog file - latest tagged release to the head"

SPEC_VERSION=`grep 'Version:' vookiimageviewer-patched.spec | cut -d' ' -f2`
cd ../../../../../../../../

echo -e  "\n\n** Creating an original package"
tar cjvf vookiimageviewer-${SPEC_VERSION}.tar.bz2 vooki-image-viewer
cp -f vookiimageviewer-${SPEC_VERSION}.tar.bz2 ~/rpmbuild/SOURCES/

echo -e  "\n\n** Creating a RPM package"
cd "${SCRIPT_LOCATION}"
rpmbuild -ba vookiimageviewer-patched.spec

echo -e  "\n\n** Listing all resulting files"
find ~/rpmbuild/ -type f -name "*.rpm"

echo -e  "\n\n** Resulting packages information"
find ~/rpmbuild/ -type f -name "*.rpm" | xargs -n1 rpm -qi || true

echo -e  "\n\n** Resulting package changelog"
find ~/rpmbuild/ -type f -name "*.rpm" | grep -v debug | grep -v '.src.' | xargs rpm -q --changelog || true

echo -e  "\n\n** Installing a resulting package"
find ~/rpmbuild/ -type f -name "*.rpm" | grep -v debug | grep -v '.src.' | xargs rpm -ivh || true

echo -e  "\n\n** Removing installed package"
rpm -e vookiimageviewer || true

rm git2changelog >>/dev/null 2>&1 || true
rm git2changelog.py >>/dev/null 2>&1 || true

echo -e  "\n\n** Done"
