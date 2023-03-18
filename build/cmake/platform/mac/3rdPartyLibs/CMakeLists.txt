CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)

PROJECT(vdeps HOMEPAGE_URL "https://vookiimageviewer.cz")

include(ExternalProject)

SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")

SET(3RD_PARTY_LIBS ${CMAKE_BINARY_DIR}/3rd-party)
SET(3RD_PARTY_LIBS_DOWNLOAD "${3RD_PARTY_LIBS}/download")
SET(3RD_PARTY_LIBS_INSTALL "${3RD_PARTY_LIBS}/install")

ExternalProject_Add(
        ext_libexiv2
        GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
        GIT_TAG v0.27.6
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES} -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DEXIV2_BUILD_SAMPLES=OFF -DEXIV2_BUILD_EXIV2_COMMAND=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libraw
        GIT_REPOSITORY https://github.com/LibRaw/LibRaw.git
        GIT_TAG 0.21.1
        GIT_SHALLOW true
        CONFIGURE_COMMAND autoreconf -fiv <SOURCE_DIR> && "${CMAKE_COMMAND}" -E echo CC="$(CC) -arch x86_64 -arch arm64" CXX="$(CXX) -arch x86_64 -arch arm64" CPPFLAGS="-I/Users/duda/Development/_scm/_my/vooki-image-viewer/build/cmake/platform/mac/3rdPartyLibs/libjpeg-turbo-2.1.4/include \${CPPFLAGS}" LDFLAGS="-L/Users/duda/Development/_scm/_my/vooki-image-viewer/build/cmake/platform/mac/3rdPartyLibs/libjpeg-turbo-2.1.4/lib \${LDFLAGS}" <SOURCE_DIR>/configure --disable-examples --enable-static=yes --enable-shared=no -prefix=${3RD_PARTY_LIBS_INSTALL} > configure.sh
        COMMAND sh configure.sh
        BUILD_COMMAND cd <BINARY_DIR> && $(MAKE) -j

        UPDATE_COMMAND ""
        DEPENDS ext_libexiv2
)
