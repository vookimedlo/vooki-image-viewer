# VookiImageViewer - a tool for showing images.
# Copyright(C) 2017-2022 Michal Duda <github@vookimedlo.cz>
#
# https://github.com/vookimedlo/vooki-image-viewer
#
# This program is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.If not, see <http://www.gnu.org/licenses/>.

PROJECT(vookiimageviewer HOMEPAGE_URL "https://vookiimageviewer.cz")

CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)

SET(APPLICATION_NAME VookiImageViewer)
MESSAGE("-- ${APPLICATION_NAME} - https://vookiimageviewer.cz")

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules ${CMAKE_MODULE_PATH})

if (NOT VERSION)
    include(GetGitRevisionDescription)
    get_git_head_revision(HEAD_REFS HEAD_HASH "ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR")
    git_describe_working_tree(HASH_TAG --tags --always --exact-match)

    SET(VERSION_REGEXP "[0-9][0-9][0-9][0-9]\\.[0-9][0-9]\\.[0-9][0-9]")
    STRING(REGEX REPLACE "^v(${VERSION_REGEXP})$" "\\1" VERSION "${HASH_TAG}")
    STRING(REGEX MATCHALL "^${VERSION_REGEXP}$" VERSION_MATCH "${VERSION}")
    if (NOT VERSION_MATCH)
        STRING(REGEX REPLACE "^(......).*$" "\\1" VERSION_HASH "${HEAD_HASH}")
        if ("${VERSION_HASH}" STREQUAL "")
            SET(VERSION_HASH "DEVEL")
        endif ()
        STRING(REGEX MATCHALL "^refs/heads/([^/]+)$" VERSION_MATCH "${HEAD_REFS}")
        if (VERSION_MATCH AND CMAKE_MATCH_COUNT EQUAL 1)
            STRING(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
            SET(VERSION "${BUILD_DATE_DOTTED}-${CMAKE_MATCH_1}-${VERSION_HASH}")
        else ()
            STRING(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
            SET(VERSION "${BUILD_DATE_DOTTED}-${VERSION_HASH}")
        endif ()
    endif ()
endif ()

MESSAGE("-- Application version is ${VERSION}")
CONFIGURE_FILE(../../src/util/version.h.in "${CMAKE_BINARY_DIR}/version.h" @ONLY)

if (APPLE)
    SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
    MESSAGE("-- Requested architectures are x86_64 & arm64")
endif ()

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

SET(QCORO_BUILD_EXAMPLES OFF)
SET(QCORO_WITH_QTDBUS OFF)
SET(QCORO_WITH_QTNETWORK OFF)
SET(QCORO_WITH_QTWEBSOCKETS OFF)
SET(QCORO_WITH_QTQUICK OFF)
SET(QCORO_WITH_QML OFF)
SET(BUILD_TESTING OFF)

FetchContent_Declare(qcoro
        GIT_REPOSITORY https://github.com/danvratil/qcoro.git
        GIT_TAG v0.8.0
        GIT_SHALLOW true
        )

FetchContent_MakeAvailable(qcoro)
qcoro_enable_coroutines()


SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_MACOSX_RPATH ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT WIN32)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 ${CMAKE_CXX_FLAGS}")
endif ()

SET(USE_QT_LIBRARIES
        Concurrent
        Core
        Gui
        Widgets)

FIND_PACKAGE(Qt6 REQUIRED COMPONENTS ${USE_QT_LIBRARIES} LinguistTools)
if (Qt6_FOUND)
    MESSAGE("-- Qt version is ${Qt6_VERSION}")

    SET(QT_UI_LIB Qt6::Gui)

    SET(QT_ALL_LIBS
            Qt6::Concurrent
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets)

    macro(QT_RESOURCES FIRST SECOND)
        QT6_ADD_RESOURCES(${FIRST} ${SECOND})
    endmacro()

    macro(QT_WRAP_UIS)
        QT6_WRAP_UI(UI_HEADERS
                ${UIS})
    endmacro()
endif ()

if (NOT LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR lib)
endif ()

# Include a path to the sources root location
#
GET_FILENAME_COMPONENT(SOURCES_ABSOLUTE_PATH ../../src/ ABSOLUTE)
SET(CMAKE_CXX_FLAGS "-I${SOURCES_ABSOLUTE_PATH} ${CMAKE_CXX_FLAGS}")

SET(SOURCES
        ../../src/application/Application.cpp
        ../../src/application/main.cpp
        ../../src/model/FileSystemSortFilterProxyModel.cpp
        ../../src/model/ImageCatalog.cpp
        ../../src/processing/ImageLoader.cpp
        ../../src/processing/ImageProcessor.cpp
        ../../src/processing/MetadataExtractor.cpp
        ../../src/ui/AboutComponentsDialog.cpp
        ../../src/ui/FileSystemTreeView.cpp
        ../../src/ui/ImageAreaWidget.cpp
        ../../src/ui/InfoTableWidget.cpp
        ../../src/ui/MainWindow.cpp
        ../../src/ui/ReleaseNotesDialog.cpp
        ../../src/ui/SettingsDialog.cpp
        ../../src/ui/SettingsShortcutsTableWidget.cpp
        ../../src/ui/StatusBar.cpp
        ../../src/ui/support/Languages.cpp
        ../../src/ui/support/RecentFileAction.cpp
        ../../src/ui/support/Settings.cpp
        ../../src/ui/support/SettingsShortcutsTableWidgetItem.cpp
        ../../src/util/ByteSize.cpp
        ../../src/util/misc.cpp
        )

SET(UIS
        ../../src/ui/forms/AboutComponentsDialog.ui
        ../../src/ui/forms/AboutDialog.ui
        ../../src/ui/forms/AboutSupportedFormatsDialog.ui
        ../../src/ui/forms/MainWindow.ui
        ../../src/ui/forms/ReleaseNotesDialog.ui
        ../../src/ui/forms/SettingsDialog.ui
        )

QT_WRAP_UIS()

# Satisfy new Policy CMP0071
#
SET_PROPERTY(SOURCE "ui_AboutComponentsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_AboutDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_AboutSupportedFormatsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_MainWindow.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_SettingsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "qrc_vookiimageviewer.cpp" PROPERTY SKIP_AUTOMOC ON)

QT_RESOURCES(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

ADD_LIBRARY(exiv2wrapper STATIC ../../src/processing/Exiv2ImageAutoPtrWrapper.cpp)
SET_TARGET_PROPERTIES(exiv2wrapper PROPERTIES CXX_STANDARD 11)
TARGET_COMPILE_OPTIONS(exiv2wrapper PRIVATE
        "$<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wno-deprecated>")

if (APPLE)
    FIND_LIBRARY(APPKIT AppKit)
    SET(OS_LIBS ${APPKIT})

    SET(MACOSX_BUNDLE_EXECUTABLE_NAME ${APPLICATION_NAME})
    SET(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${APPLICATION_NAME})
    SET(MACOSX_BUNDLE_COPYRIGHT "Michal Duda")
    SET(MACOSX_BUNDLE_INFO_STRING "Lightweight image viewer")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER "cz.VookiImageViewer")

    SET_SOURCE_FILES_PROPERTIES(../../src/resource/openclipart/vookiimageviewericon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    SET(SOURCES
            ${SOURCES}
            ../../src/abstraction/mac/init.mm
            ../../src/abstraction/mac/darkmode.mm
            ../../components/KDMacTouchBar/src/kdmactouchbar.mm
            )

    GET_FILENAME_COMPONENT(COMPONENTS_ABSOLUTE_PATH ../../components/KDMacTouchBar/src ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${COMPONENTS_ABSOLUTE_PATH} ${CMAKE_CXX_FLAGS}")

    ADD_EXECUTABLE(${APPLICATION_NAME}
            MACOSX_BUNDLE
            ../../src/resource/openclipart/vookiimageviewericon.icns
            ${SOURCES}
            ${UI_HEADERS}
            ${UI_RESOURCES})

    GET_FILENAME_COMPONENT(PLIST_IN_ABSOLUTE_PATH platform/mac/support/MacOSXBundleInfo.plist.in ABSOLUTE)
    SET_TARGET_PROPERTIES(${APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_IN_ABSOLUTE_PATH})

    TARGET_COMPILE_OPTIONS(${APPLICATION_NAME} PRIVATE
            "$<$<OR:$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>,$<CONFIG:MinSizeRel>>:-DQT_NO_DEBUG_OUTPUT>")

    FIND_PACKAGE(ZLIB REQUIRED)
    FIND_PACKAGE(Iconv REQUIRED)
    FIND_PACKAGE(EXPAT REQUIRED)

    SET(EXIV2_LOC "3rdPartyLibs/Exiv2-0.27.6")
    GET_FILENAME_COMPONENT(EXIV2 platform/mac/${EXIV2_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${EXIV2}/include ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "-L${EXIV2}/lib ${CMAKE_EXE_LINKER_FLAGS}")

    TARGET_LINK_LIBRARIES(VookiImageViewer libexiv2.a libexiv2-xmp.a ZLIB::ZLIB Iconv::Iconv EXPAT::EXPAT)
elseif (WIN32)
    SET(SOURCES
            ${SOURCES}
            ../../src/abstraction/win/init.cpp
            ../../src/abstraction/win/darkmode.cpp
            )

    ADD_EXECUTABLE(${APPLICATION_NAME}
            WIN32
            platform/windows/support/application.rc
            ${SOURCES}
            ${UI_HEADERS}
            ${UI_RESOURCES})

    SET(EXIV2_LOC "3rdPartyLibs/Exiv2-0.27.6")
    GET_FILENAME_COMPONENT(EXIV2 platform/windows/${EXIV2_LOC} ABSOLUTE)
    SET(ZLIB_LOC "3rdPartyLibs/zlib-1.2.13")
    GET_FILENAME_COMPONENT(ZLIB platform/windows/${ZLIB_LOC} ABSOLUTE)
    SET(EXPAT_LOC "3rdPartyLibs/expat-2.5.0")
    GET_FILENAME_COMPONENT(EXPAT platform/windows/${EXPAT_LOC} ABSOLUTE)

    SET(CMAKE_CXX_FLAGS "-I${EXIV2}/include -I${ZLIB}/include -I${EXPAT}/include ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "/LIBPATH:${EXIV2}/lib /LIBPATH:${ZLIB}/lib /LIBPATH:${EXPAT}/lib ${CMAKE_EXE_LINKER_FLAGS}")

    TARGET_LINK_LIBRARIES(VookiImageViewer exiv2.lib exiv2-xmp.lib zlibstatic.lib libexpatMD.lib wsock32 ws2_32 Psapi.lib exiv2wrapper)
else ()
    SET(SOURCES
            ${SOURCES}
            ../../src/abstraction/unix/init.cpp
            ../../src/abstraction/unix/darkmode.cpp
            )

    ADD_EXECUTABLE(VookiImageViewer
            ${SOURCES}
            ${UI_HEADERS}
            ${UI_RESOURCES})

    FIND_LIBRARY(LIBEXIV2 NAMES libexiv2 exiv2 REQUIRED)
    TARGET_LINK_LIBRARIES(VookiImageViewer ${LIBEXIV2})
endif ()

TARGET_LINK_LIBRARIES(VookiImageViewer ${QT_ALL_LIBS})
TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})
TARGET_LINK_LIBRARIES(VookiImageViewer exiv2wrapper)
TARGET_LINK_LIBRARIES(VookiImageViewer QCoro6Core)

#### Translations ####
#

function(ADD_TRANSLATION lang_code lang_name)
    SET(TRANSLATION_FILES ${TRANSLATION_FILES} ../../src/resource/translations/${APPLICATION_NAME}_${lang_code}.ts PARENT_SCOPE)
    SET(TRANSLATION_LANGUAGES ${TRANSLATION_LANGUAGES} ${lang_code} PARENT_SCOPE)
    SET(TRANSLATION_STRUCT_RECORDS ${TRANSLATION_STRUCT_RECORDS} "{\"${lang_code}\", \"${lang_name}\"}, " PARENT_SCOPE)
endfunction()

ADD_TRANSLATION(en_US "English (US)")
ADD_TRANSLATION(cs "Čeština — Czech")
ADD_TRANSLATION(da "Dansk — Danish")
ADD_TRANSLATION(de "Deutsch — German")
ADD_TRANSLATION(el "Ελληνικά — Greek")
ADD_TRANSLATION(es "Español — Spanish")
ADD_TRANSLATION(et "Eesti — Estonian")
ADD_TRANSLATION(fi "Suomi — Finnish")
ADD_TRANSLATION(fr "Français — French")
ADD_TRANSLATION(hr "Hrvatski — Croatian")
ADD_TRANSLATION(hu "Magyar — Hungarian")
ADD_TRANSLATION(ja "日本語 — Japanese")
ADD_TRANSLATION(nl "Nederlands — Dutch")
ADD_TRANSLATION(ro "Română — Romanian")
ADD_TRANSLATION(ru "Русский — Russian")
ADD_TRANSLATION(sk "Slovenčina — Slovak")
ADD_TRANSLATION(sl "Slovenščina — Slovenian")
ADD_TRANSLATION(sv "Svenska — Swedish")
ADD_TRANSLATION(tr "Türkçe — Turkish")
ADD_TRANSLATION(uk "Українська — Ukrainian")
ADD_TRANSLATION(vi "Tiếng Việt — Vietnamese")
ADD_TRANSLATION(zh_CN "简体中文 — Simplified Chinese")

STRING(REPLACE ";" "\n" TRANSLATION_STRUCT_RECORDS_STRING "${TRANSLATION_STRUCT_RECORDS}")
CONFIGURE_FILE(../../src/ui/support/Languages_m_localizations.h.in "${CMAKE_BINARY_DIR}/Languages_m_localizations.h" @ONLY)

qt6_add_lupdate(${APPLICATION_NAME}
        TS_FILES ${TRANSLATION_FILES}
        SOURCES ${SOURCES} ${UIS} ../../src/processing/MetadataExtractor.h)
qt6_add_lrelease(${APPLICATION_NAME}
        TS_FILES ${TRANSLATION_FILES}
        QM_FILES_OUTPUT_VARIABLE qm_files)
qt6_add_resources(${APPLICATION_NAME} "translations"
        PREFIX "/i18n"
        BASE "${CMAKE_BINARY_DIR}"
        FILES "${qm_files}")

#### Plugins ####
#

function(ADD_IMAGE_PLUGIN name source)
    ADD_LIBRARY(${name} MODULE ${source} ${ARGN})
    TARGET_LINK_LIBRARIES(${name} ${QT_UI_LIB})
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} ${name} PARENT_SCOPE)
endfunction()


# Personal Computer Exchange (pcx)
#
ADD_IMAGE_PLUGIN(vooki_kimg_pcx ../../src/plugins/kimageformats/pcx.cpp)

# Gimp (xcf)
#
ADD_IMAGE_PLUGIN(vooki_kimg_xcf ../../src/plugins/kimageformats/xcf.cpp)

# Targa (tga): supports more formats than Qt's version
#
ADD_IMAGE_PLUGIN(vooki_kimg_tga ../../src/plugins/kimageformats/tga.cpp)

# Photoshop documents (psd)
#
ADD_IMAGE_PLUGIN(vooki_kimg_psd ../../src/plugins/kimageformats/psd.cpp)

# Windows Animated Cursor (ani)
#
ADD_IMAGE_PLUGIN(vooki_kimg_ani ../../src/plugins/kimageformats/ani.cpp)

# Radiance HDR (hdr)
#
ADD_IMAGE_PLUGIN(vooki_kimg_hdr ../../src/plugins/kimageformats/hdr.cpp)

# Sun Raster (ras)
#
ADD_IMAGE_PLUGIN(vooki_kimg_ras ../../src/plugins/kimageformats/ras.cpp)

# Softimage PIC (pic)
#
ADD_IMAGE_PLUGIN(vooki_kimg_pic ../../src/plugins/kimageformats/pic.cpp)

# SGI images (rgb, rgba, sgi, bw)
#
ADD_IMAGE_PLUGIN(vooki_kimg_rgb ../../src/plugins/kimageformats/rgb.cpp)

# Camera RAW Thumbnails (raf, mos, cr2, erf, dng, mrw, nef, orf, rw2, pef, x3f, srw, x3f, arw)
# Prebuilt library for the MacOS/Windows only, Linux uses a distro package
#
if (APPLE)
    # MacOS
    #
    SET(3RD_LIB_LOC "platform/mac/3rdPartyLibs")

    SET(LIBJPEG_LOC "libjpeg-turbo-2.1.4")
    GET_FILENAME_COMPONENT(LIBJPEG "${3RD_LIB_LOC}/${LIBJPEG_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBJPEG}/include ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L${LIBJPEG}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    SET(LIBRAW_LOC "LibRaw-0.21.0")
    GET_FILENAME_COMPONENT(LIBRAW "${3RD_LIB_LOC}/${LIBRAW_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW}/include ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L${LIBRAW}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_raw_thumb ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} libraw_r.a libturbojpeg.a -lz)

    include(platform/mac/MacdeployQt.cmake)
    deployqt()

    INCLUDE(platform/mac/CPack.cmake)
elseif (WIN32)
    # Windows
    #
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    SET(3RD_LIB_LOC "platform/windows/3rdPartyLibs")

    SET(ZLIB_LOC "zlib-1.2.13")
    GET_FILENAME_COMPONENT(ZLIB "${3RD_LIB_LOC}/${ZLIB_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${ZLIB}/include ${CMAKE_CXX_FLAGS}")

    SET(LIBJPEG_LOC "libjpeg-turbo-2.1.4")
    GET_FILENAME_COMPONENT(LIBJPEG "${3RD_LIB_LOC}/${LIBJPEG_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBJPEG}/include ${CMAKE_CXX_FLAGS}")

    SET(LIBRAW_LOC "LibRaw-0.21.0")
    ADD_DEFINITIONS(-DLIBRAW_NODLL)
    GET_FILENAME_COMPONENT(LIBRAW "${3RD_LIB_LOC}/${LIBRAW_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW}/include ${CMAKE_CXX_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_raw_thumb ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW}/lib/libraw_static.lib ${LIBJPEG}/lib/turbojpeg-static.lib ${ZLIB}/lib/zlibstatic.lib)

    SET(LIBRAW_LOC "libheif-1.14.0")
    GET_FILENAME_COMPONENT(LIBHEIF "${3RD_LIB_LOC}/${LIBRAW_LOC}" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBHEIF}/include ${CMAKE_CXX_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_kimg_heif ../../src/plugins/kimageformats/heif.cpp)
    TARGET_LINK_LIBRARIES(vooki_kimg_heif ${QT_UI_LIB} ${LIBHEIF}/lib/heif.lib)

    SET(LIBDE265_LOC "libde265-1.0.9")
    GET_FILENAME_COMPONENT(LIBDE265 "${3RD_LIB_LOC}/${LIBDE265_LOC}" ABSOLUTE)

    SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/imageformats")
    SET_TARGET_PROPERTIES(${IMAGE_PLUGINS}
            PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGINS})

    ADD_CUSTOM_TARGET(copy-heif-deps)
    GET_FILENAME_COMPONENT(LIBDE265 "${LIBDE265}/lib/libde265.dll" ABSOLUTE)
    GET_FILENAME_COMPONENT(LIBHEIF "${LIBHEIF}/lib/heif.dll" ABSOLUTE)
    ADD_CUSTOM_COMMAND(TARGET copy-heif-deps PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${LIBDE265} ${LIBHEIF} $<TARGET_FILE_DIR:${APPLICATION_NAME}>)
    ADD_DEPENDENCIES(vooki_kimg_heif copy-heif-deps)
    ADD_DEPENDENCIES(copy-heif-deps ${APPLICATION_NAME})

    include(platform/windows/WindeployQt.cmake)
    deployqt()

    INCLUDE(platform/windows/CPack.cmake)
else ()
    # Unix-like systems
    #
    MESSAGE("-- ${APPLICATION_NAME} expects its plugins located either in /usr/lib/vookiimageviewer/imageformats or in /usr/local/lib/vookiimageviewer/imageformats")
    ADD_DEFINITIONS(-DUNIX_LIKE)

    FIND_LIBRARY(LIBRAW NAMES libraw_r raw_r)
    if (LIBRAW)
        ADD_IMAGE_PLUGIN(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
        TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW})
    endif ()

    INCLUDE(platform/unix/CPack.cmake)
endif ()
