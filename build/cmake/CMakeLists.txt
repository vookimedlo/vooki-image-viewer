# VookiImageViewer - tool to showing images.
# Copyright(C) 2017-2019 Michal Duda <github@vookimedlo.cz>
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

CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(vookiimageviewer)

MESSAGE("-- VookiImageViewer - https://vookiimageviewer.cz")

SET(USE_QT_LIBRARIES
    Core Gui Widgets)

FIND_PACKAGE(Qt5 REQUIRED ${USE_QT_LIBRARIES})


if (Qt5_FOUND)
    MESSAGE("-- Qt version is ${Qt5_VERSION}")
endif()

# std::make_unique is part of c++14
#
SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

if (NOT WIN32)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if (NOT BUILD_DATE)
    STRING(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    STRING(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
endif()

ADD_DEFINITIONS(
    -DBUILD_DATE=\"${BUILD_DATE_DOTTED}\"
)

# Include a path to the sources root location
#
GET_FILENAME_COMPONENT(SOURCES_ABSOLUTE_PATH ../../src/ ABSOLUTE)
SET(CMAKE_CXX_FLAGS "-I${SOURCES_ABSOLUTE_PATH} ${CMAKE_CXX_FLAGS}")

SET(SOURCES
    ../../src/application/Application.cpp
    ../../src/application/main.cpp
    ../../src/model/FileSystemSortFilterProxyModel.cpp
    ../../src/model/ImageCatalog.cpp
    ../../src/ui/AboutComponentsDialog.cpp
    ../../src/ui/FileSystemTreeView.cpp
    ../../src/ui/ImageAreaWidget.cpp
    ../../src/ui/MainWindow.cpp
    ../../src/ui/SettingsDialog.cpp
    ../../src/ui/SettingsShortcutsTableWidget.cpp
    ../../src/ui/StatusBar.cpp
    ../../src/ui/support/RecentFileAction.cpp
    ../../src/ui/support/Settings.cpp
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.cpp
    ../../src/util/misc.cpp
)

QT5_WRAP_UI(UI_HEADERS
    ../../src/ui/forms/AboutComponentsDialog.ui
    ../../src/ui/forms/AboutDialog.ui
    ../../src/ui/forms/AboutSupportedFormatsDialog.ui
    ../../src/ui/forms/MainWindow.ui
    ../../src/ui/forms/ReleaseNotesDialog.ui
    ../../src/ui/forms/SettingsDialog.ui
)

# Satisfy new Policy CMP0071
#
SET_PROPERTY(SOURCE "ui_AboutComponentsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_AboutDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_AboutSupportedFormatsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_MainWindow.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "ui_SettingsDialog.h" PROPERTY SKIP_AUTOMOC ON)
SET_PROPERTY(SOURCE "qrc_vookiimageviewer.cpp" PROPERTY SKIP_AUTOMOC ON)

QT5_ADD_RESOURCES(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

if (APPLE)
    FIND_LIBRARY(APPKIT AppKit)
    SET(OS_LIBS ${APPKIT})

    SET(MACOSX_BUNDLE_EXECUTABLE_NAME VookiImageViewer)
    SET(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    SET(MACOSX_BUNDLE_BUNDLE_NAME VookiImageViewer)
    SET(MACOSX_BUNDLE_COPYRIGHT "(C) 2019 Michal Duda")
    SET(MACOSX_BUNDLE_INFO_STRING "Lightweight image viewer")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${BUILD_DATE_DOTTED})
    SET(MACOSX_BUNDLE_GUI_IDENTIFIER "cz.VookiImageViewer")

    SET_SOURCE_FILES_PROPERTIES(../../src/resource/openclipart/vookiimageviewericon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    SET(SOURCES
        ${SOURCES}
        ../../src/abstraction/mac/init.mm
        ../../src/abstraction/mac/darkmode.mm
    )

    ADD_EXECUTABLE(VookiImageViewer
        MACOSX_BUNDLE
        ../../src/resource/openclipart/vookiimageviewericon.icns
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})

    GET_FILENAME_COMPONENT(PLIST_IN_ABSOLUTE_PATH mac/support/MacOSXBundleInfo.plist.in ABSOLUTE)
    SET_TARGET_PROPERTIES(VookiImageViewer PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_IN_ABSOLUTE_PATH})

elseif(WIN32)
    SET(SOURCES
        ${SOURCES}
        ../../src/abstraction/win/init.cpp
        ../../src/abstraction/win/darkmode.cpp
    )

    ADD_EXECUTABLE(VookiImageViewer
        WIN32
        windows/support/application.rc
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})
else()
    SET(SOURCES
        ${SOURCES}
        ../../src/abstraction/unix/init.cpp
        ../../src/abstraction/unix/darkmode.cpp
    )

    ADD_EXECUTABLE(VookiImageViewer
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})
endif()

TARGET_LINK_LIBRARIES(VookiImageViewer
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets)

TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})


#### Plugins ####
#

# Photoshop documents (psd)
#
ADD_LIBRARY(vooki_kimg_psd MODULE ../../src/plugins/kimageformats/psd.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_psd Qt5::Gui)

# Personal Computer Exchange (pcx)
#
ADD_LIBRARY(vooki_kimg_pcx MODULE ../../src/plugins/kimageformats/pcx.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pcx Qt5::Gui)

if (Qt5_FOUND)
    if (Qt5_VERSION VERSION_GREATER_EQUAL 5.8.0)
        # Q_FALLTHROUGH was introduced in 5.8.0
        #

        # Gimp (xcf)
        #
        ADD_LIBRARY(vooki_kimg_xcf MODULE ../../src/plugins/kimageformats/xcf.cpp)
        TARGET_LINK_LIBRARIES(vooki_kimg_xcf Qt5::Gui)

        # Targa (tga): supports more formats than Qt's version
        #
        ADD_LIBRARY(vooki_kimg_tga MODULE ../../src/plugins/kimageformats/tga.cpp)
        TARGET_LINK_LIBRARIES(vooki_kimg_tga Qt5::Gui)
    endif()
endif()

# Sun Raster (ras)
#
ADD_LIBRARY(vooki_kimg_ras MODULE ../../src/plugins/kimageformats/ras.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_ras Qt5::Gui)

# Softimage PIC (pic)
#
ADD_LIBRARY(vooki_kimg_pic MODULE ../../src/plugins/kimageformats/pic.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pic Qt5::Gui)

# SGI images (rgb, rgba, sgi, bw)
#
ADD_LIBRARY(vooki_kimg_rgb MODULE ../../src/plugins/kimageformats/rgb.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_rgb Qt5::Gui)

# Camera RAW Thumbnails (raf, mos, cr2, erf, dng, mrw, nef, orf, rw2, pef, x3f, srw, x3f, arw)
# Prebuilt library for the MacOS/Windows only, Linux uses a distro package
#
SET(LIBRAW_LOC "3rdPartyLibs/LibRaw-0.19.2")
if (APPLE)
    # MacOS
    #
    GET_FILENAME_COMPONENT(LIBRAW mac/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L${LIBRAW}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui libraw_r.a)
elseif(WIN32)
    # Windows
    #
    ADD_DEFINITIONS(-DLIBRAW_NODLL)

    GET_FILENAME_COMPONENT(LIBRAW windows/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")

    ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui ${LIBRAW}/lib/libraw.lib)

    GET_FILENAME_COMPONENT(LIBHEIF "windows/3rdPartyLibs/libHEIF-1.4.0" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBHEIF} ${CMAKE_CXX_FLAGS}")

    ADD_LIBRARY(vooki_heic_image MODULE ../../src/plugins/heicimage/heicHandler.cpp ../../src/plugins/heicimage/heicPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_heic_image Qt5::Gui ${LIBHEIF}/lib/libheif.lib)
else()
    # Unix-like systems
    #
    MESSAGE("-- VookiImageViewer expects its plugins located either in /usr/lib/vookiimageviewer/imageformats or /usr/local/lib/vookiimageviewer/imageformats")
    SET(PLUGINS "lib/vookiimageviewer/imageformats")
    ADD_DEFINITIONS(-DUNIX_LIKE)

    FIND_LIBRARY(LIBRAW NAMES libraw_r raw_r)
    if (LIBRAW)
        ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
        TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui ${LIBRAW})
    endif()

    INSTALL(TARGETS VookiImageViewer vooki_kimg_psd vooki_kimg_pcx vooki_kimg_ras vooki_kimg_pic vooki_kimg_rgb
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION ${PLUGINS})

    GET_FILENAME_COMPONENT(TOP_LEVEL_ABSOLUTE_PATH ../../ ABSOLUTE)
    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-IMAGES
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-OPENCLIPART
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-PLUGINS-KIMAGEFORMATS
                  ${TOP_LEVEL_ABSOLUTE_PATH}/README.md
            DESTINATION share/doc/vookiimageviewer)

    if (TARGET vooki_kimg_xcf)
        INSTALL(TARGETS vooki_kimg_xcf
                LIBRARY DESTINATION ${PLUGINS})
    endif()

    if (TARGET vooki_kimg_tga)
        INSTALL(TARGETS vooki_kimg_tga
                LIBRARY DESTINATION ${PLUGINS})
    endif()

    if (TARGET vooki_raw_thumb)
        INSTALL(TARGETS vooki_raw_thumb
                LIBRARY DESTINATION ${PLUGINS})
    endif()
endif()
