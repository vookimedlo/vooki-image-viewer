#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)

PROJECT(vdeps HOMEPAGE_URL "https://vookiimageviewer.cz")

include(../../../ExternalProjectVersions.cmake)
include(ExternalProject)

SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
SET(3RD_PARTY_LIBS_DOWNLOAD "${3RD_PARTY_LIBS}/download")

ExternalProject_Add(
        ext_brotli
        GIT_REPOSITORY https://github.com/google/brotli.git
        GIT_TAG ${EXT_BROTLI_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libexiv2
        GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
        GIT_TAG ${EXT_LIBEXIV2_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DEXIV2_BUILD_SAMPLES=OFF -DEXIV2_BUILD_EXIV2_COMMAND=OFF -DEXIV2_ENABLE_INIH=OFF -DEXIV2_ENABLE_VIDEO=OFF -DEXIV2_ENABLE_BROTLI=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libjpeg-turbo
        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
        GIT_TAG ${EXT_LIBJPEG-TURBO_VERSION}
        GIT_SHALLOW true
        CONFIGURE_COMMAND ""
        COMMAND mkdir <BINARY_DIR>/build-x86_64 || true &&
                cd <BINARY_DIR>/build-x86_64 &&
                $(CMAKE_COMMAND) -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF -DENABLE_SHARED=OFF -DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/build-x86_64/install <SOURCE_DIR> &&
                $(MAKE) -j &&
                $(MAKE) install
        COMMAND mkdir <BINARY_DIR>/build-arm64 || true &&
                cd <BINARY_DIR>/build-arm64 &&
                $(CMAKE_COMMAND) -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF -DENABLE_SHARED=OFF -DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/build-arm64/install <SOURCE_DIR> &&
                $(MAKE) -j &&
                $(MAKE) install
        BUILD_COMMAND lipo -create build-arm64/install/lib/libjpeg.a build-x86_64/install/lib/libjpeg.a -o libjpeg.a &&
                      lipo -create build-arm64/install/lib/libturbojpeg.a build-x86_64/install/lib/libturbojpeg.a -o libturbojpeg.a
        INSTALL_COMMAND cp -rf build-arm64/install/include/ ${3RD_PARTY_LIBS_INSTALL}/include/ &&
                        cp -f libjpeg.a libturbojpeg.a ${3RD_PARTY_LIBS_INSTALL}/lib/ &&
                        cp -rf build-arm64/install/lib/cmake/ ${3RD_PARTY_LIBS_INSTALL}/lib/cmake/
        UPDATE_COMMAND ""
        DEPENDS ext_libexiv2
)

ExternalProject_Add(
        ext_libraw
        GIT_REPOSITORY https://github.com/LibRaw/LibRaw.git
        GIT_TAG ${EXT_LIBRAW_VERSION}
        GIT_SHALLOW true
        CONFIGURE_COMMAND autoreconf -fiv <SOURCE_DIR> && "${CMAKE_COMMAND}" -E echo CC="$(CC) -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -arch x86_64 -arch arm64" CXX="$(CXX) -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -arch x86_64 -arch arm64" CPPFLAGS="-I${3RD_PARTY_LIBS_INSTALL}/include -DLIBRAW_CALLOC_RAWSTORE \${CPPFLAGS}" LDFLAGS="-L${3RD_PARTY_LIBS_INSTALL}/lib \${LDFLAGS}" <SOURCE_DIR>/configure --disable-examples --enable-static=yes --enable-shared=no -prefix=${3RD_PARTY_LIBS_INSTALL} > configure.sh
        COMMAND sh configure.sh
        BUILD_COMMAND cd <BINARY_DIR> && $(MAKE) -j
        UPDATE_COMMAND ""
        DEPENDS ext_libjpeg-turbo
)

ExternalProject_Add(
        ext_highway
        GIT_REPOSITORY https://github.com/google/highway.git
        GIT_TAG ${EXT_HIGHWAY_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DHWY_ENABLE_TESTS=off -DHWY_ENABLE_EXAMPLES=off -DHWY_ENABLE_CONTRIB=off -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libjxl
        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG ${EXT_LIBJXL_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_OSX_DEPLOYMENT_TARGET:STRING=${CMAKE_OSX_DEPLOYMENT_TARGET} -DCMAKE_HAVE_LIBC_PTHREAD=true -DATOMICS_LOCK_FREE_INSTRUCTIONS=true -DJPEGXL_FORCE_SYSTEM_HWY=true -DHWY_INCLUDE_DIR=${3RD_PARTY_LIBS_INSTALL}/include -DJPEGXL_FORCE_SYSTEM_BROTLI=true -DBROTLI_INCLUDE_DIR=${3RD_PARTY_LIBS_INSTALL}/include -DJPEGXL_ENABLE_FUZZERS=false -DJPEGXL_ENABLE_DEVTOOLS=false -DJPEGXL_ENABLE_TOOLS=false -DJPEGXL_ENABLE_JPEGLI=false -DJPEGXL_ENABLE_JPEGLI_LIBJPEG=false -DJPEGXL_ENABLE_DOXYGEN=false -DJPEGXL_ENABLE_MANPAGES=false -DJPEGXL_ENABLE_BENCHMARK=false -DJPEGXL_ENABLE_EXAMPLES=false -DJPEGXL_BUNDLE_LIBPNG=false -DJPEGXL_ENABLE_JNI=false -DJPEGXL_ENABLE_SJPEG=false -DJPEGXL_ENABLE_OPENEXR=false -DJPEGXL_ENABLE_SKCMS=true -DJPEGXL_ENABLE_VIEWERS=false -DJPEGXL_ENABLE_TCMALLOC=false -DJPEGXL_ENABLE_PLUGINS=false -DJPEGXL_ENABLE_COVERAGE=false -DJPEGXL_ENABLE_SIZELESS_VECTORS=false -DJPEGXL_ENABLE_TRANSCODE_JPEG=true -DJPEGXL_ENABLE_BOXES=true -DJPEGXL_STATIC=true -DCMAKE_PREFIX_PATH=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF
        UPDATE_COMMAND ""
        DEPENDS ext_brotli ext_highway
)
