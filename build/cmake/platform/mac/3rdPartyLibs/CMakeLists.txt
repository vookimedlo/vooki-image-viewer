CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)

PROJECT(vdeps HOMEPAGE_URL "https://vookiimageviewer.cz")

include(ExternalProject)

SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")

SET(3RD_PARTY_LIBS_DOWNLOAD "${3RD_PARTY_LIBS}/download")

ExternalProject_Add(
        ext_libexiv2
        GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
        GIT_TAG v0.27.6
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=${CMAKE_OSX_ARCHITECTURES} -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DEXIV2_BUILD_SAMPLES=OFF -DEXIV2_BUILD_EXIV2_COMMAND=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libjpeg-turbo
        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
        GIT_TAG 2.1.91
        GIT_SHALLOW true

        CONFIGURE_COMMAND ""
        COMMAND mkdir <BINARY_DIR>/build-x86_64 || true &&
                cd <BINARY_DIR>/build-x86_64 &&
                $(CMAKE_COMMAND) -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF -DENABLE_SHARED=OFF -DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/build-x86_64/install <SOURCE_DIR> &&
                $(MAKE) -j &&
                $(MAKE) install
        COMMAND mkdir <BINARY_DIR>/build-arm64 || true &&
                cd <BINARY_DIR>/build-arm64 &&
                $(CMAKE_COMMAND) -DCMAKE_OSX_ARCHITECTURES="arm64" -DCMAKE_BUILD_WITH_INSTALL_RPATH=OFF -DENABLE_SHARED=OFF -DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/build-arm64/install <SOURCE_DIR> &&
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
        GIT_TAG 0.21.1
        GIT_SHALLOW true
        CONFIGURE_COMMAND autoreconf -fiv <SOURCE_DIR> && "${CMAKE_COMMAND}" -E echo CC="$(CC) -arch x86_64 -arch arm64" CXX="$(CXX) -arch x86_64 -arch arm64" CPPFLAGS="-I${3RD_PARTY_LIBS_INSTALL}/include \${CPPFLAGS}" LDFLAGS="-L${3RD_PARTY_LIBS_INSTALL}/lib \${LDFLAGS}" <SOURCE_DIR>/configure --disable-examples --enable-static=yes --enable-shared=no -prefix=${3RD_PARTY_LIBS_INSTALL} > configure.sh
        COMMAND sh configure.sh
        BUILD_COMMAND cd <BINARY_DIR> && $(MAKE) -j

        UPDATE_COMMAND ""
        DEPENDS ext_libjpeg-turbo
)
