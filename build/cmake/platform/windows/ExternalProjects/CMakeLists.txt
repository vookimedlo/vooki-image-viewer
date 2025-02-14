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

SET(3RD_PARTY_LIBS_DOWNLOAD "${3RD_PARTY_LIBS}/download")
SET_PROPERTY(DIRECTORY PROPERTY EP_BASE "c:/_tmp")

if (CMAKE_BUILD_TYPE)
    SET(CONFIG_ARGS "--config ${CMAKE_BUILD_TYPE}")
else()
    SET(CONFIG_ARGS "--config $<CONFIG>")
endif()

MESSAGE("-- Configuration arguments: ${CONFIG_ARGS}")

ExternalProject_Add(
        ext_zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG ${EXT_ZLIB_VERSION}
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DBUILD_SHARED_LIBS=OFF
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_brotli
        GIT_REPOSITORY https://github.com/google/brotli.git
        GIT_TAG ${EXT_BROTLI_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libexpat
        GIT_REPOSITORY https://github.com/libexpat/libexpat.git
        GIT_TAG ${EXT_LIBEXPAT_VERSION}
        GIT_SHALLOW true

        SOURCE_SUBDIR expat
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DEXPAT_ENABLE_INSTALL=ON -DEXPAT_SHARED_LIBS=OFF -DEXPAT_BUILD_DOCS=OFF -DEXPAT_BUILD_TESTS=OFF
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libexiv2
        GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
        GIT_TAG ${EXT_LIBEXIV2_VERSION}
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_FIND_ROOT_PATH=${3RD_PARTY_LIBS_INSTALL} -DBUILD_SHARED_LIBS=OFF -DEXIV2_BUILD_SAMPLES=OFF -DEXIV2_BUILD_EXIV2_COMMAND=OFF -DEXIV2_ENABLE_INIH=OFF -DEXIV2_ENABLE_VIDEO=OFF -DEXIV2_ENABLE_BROTLI=OFF -DEXPAT_LIBRARY="${3RD_PARTY_LIBS_INSTALL}/lib/libexpatMD.lib"
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
        DEPENDS ext_libexpat ext_zlib
)

ExternalProject_Add(
        ext_libjpeg-turbo
        GIT_REPOSITORY https://github.com/libjpeg-turbo/libjpeg-turbo.git
        GIT_TAG ${EXT_LIBJPEG-TURBO_VERSION}
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DENABLE_SHARED=OFF -DWITH_CRT_DLL=ON
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libde265
        GIT_REPOSITORY https://github.com/strukturag/libde265.git
        GIT_TAG ${EXT_LIBDE265_VERSION}
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DBUILD_SHARED_LIBS=ON -DENABLE_SDL=OFF -DENABLE_DECODER=OFF -DENABLE_ENCODER=OFF
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libheif
        GIT_REPOSITORY https://github.com/strukturag/libheif.git
        GIT_TAG ${EXT_LIBHEIF_VERSION}
        GIT_SHALLOW true

        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DBUILD_SHARED_LIBS=OFF -DENABLE_PLUGIN_LOADING=OFF -DWITH_AOM_DECODER=OFF -DWITH_AOM_ENCODER=OFF -DWITH_DAV1D=OFF -DWITH_LIBDE265=ON -DWITH_RAV1E=OFF -DWITH_X265=OFF -DWITH_SvtEnc=OFF -DWITH_EXAMPLES=OFF
        BUILD_COMMAND ${CMAKE_COMMAND} --build . ${CONFIG_ARGS}
        UPDATE_COMMAND ""
        DEPENDS ext_libde265
)

ExternalProject_Add(
        ext_libraw
        GIT_REPOSITORY https://github.com/LibRaw/LibRaw.git
        GIT_TAG ${EXT_LIBRAW_VERSION}
        GIT_SHALLOW true

        CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE ON
        BUILD_COMMAND SET JPEG_DEF=/DUSE_JPEG /DUSE_ZLIB /DLIBRAW_CALLOC_RAWSTORE /I ${3RD_PARTY_LIBS_INSTALL}/include && nmake -f <SOURCE_DIR>/Makefile.msvc lib/libraw_static.lib
        INSTALL_COMMAND (robocopy lib ${3RD_PARTY_LIBS_INSTALL}/lib *.lib || tree lib) && (robocopy libraw ${3RD_PARTY_LIBS_INSTALL}/include/libraw/ *.h || tree libraw)
        UPDATE_COMMAND ""
        DEPENDS ext_libjpeg-turbo ext_zlib
)

ExternalProject_Add(
        ext_highway
        GIT_REPOSITORY https://github.com/google/highway.git
        GIT_TAG ${EXT_HIGHWAY_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DHWY_ENABLE_TESTS=off -DHWY_ENABLE_EXAMPLES=off -DHWY_ENABLE_CONTRIB=off -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF
        UPDATE_COMMAND ""
)

ExternalProject_Add(
        ext_libjxl
        GIT_REPOSITORY https://github.com/libjxl/libjxl.git
        GIT_TAG ${EXT_LIBJXL_VERSION}
        GIT_SHALLOW true
        LIST_SEPARATOR |
        CMAKE_ARGS -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL -DJPEGXL_FORCE_SYSTEM_HWY=true -DHWY_INCLUDE_DIR=${3RD_PARTY_LIBS_INSTALL}/include -DJPEGXL_FORCE_SYSTEM_BROTLI=true -DBROTLI_INCLUDE_DIR=${3RD_PARTY_LIBS_INSTALL}/include -DJPEGXL_ENABLE_FUZZERS=false -DJPEGXL_ENABLE_DEVTOOLS=false -DJPEGXL_ENABLE_TOOLS=false -DJPEGXL_ENABLE_JPEGLI=false -DJPEGXL_ENABLE_JPEGLI_LIBJPEG=false -DJPEGXL_ENABLE_DOXYGEN=false -DJPEGXL_ENABLE_MANPAGES=false -DJPEGXL_ENABLE_BENCHMARK=false -DJPEGXL_ENABLE_EXAMPLES=false -DJPEGXL_BUNDLE_LIBPNG=false -DJPEGXL_ENABLE_JNI=false -DJPEGXL_ENABLE_SJPEG=false -DJPEGXL_ENABLE_OPENEXR=false -DJPEGXL_ENABLE_SKCMS=true -DJPEGXL_ENABLE_VIEWERS=false -DJPEGXL_ENABLE_TCMALLOC=false -DJPEGXL_ENABLE_PLUGINS=false -DJPEGXL_ENABLE_COVERAGE=false -DJPEGXL_ENABLE_SIZELESS_VECTORS=false -DJPEGXL_ENABLE_TRANSCODE_JPEG=true -DJPEGXL_ENABLE_BOXES=true -DJPEGXL_STATIC=true -DCMAKE_PREFIX_PATH=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_OSX_ARCHITECTURES:STRING=x86_64|arm64 -DCMAKE_INSTALL_PREFIX=${3RD_PARTY_LIBS_INSTALL} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF
        UPDATE_COMMAND ""
        DEPENDS ext_brotli ext_highway
)
