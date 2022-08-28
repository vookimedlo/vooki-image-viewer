# VookiImageViewer - tool to showing images.
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

PROJECT(vookiimageviewer)

CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

set(APPLICATION_NAME VookiImageViewer)
MESSAGE("-- ${APPLICATION_NAME} - https://vookiimageviewer.cz")


if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
    MESSAGE("-- Requested architectures are x86_64 & arm64")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT WIN32)
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2 ${CMAKE_CXX_FLAGS}")
endif()

SET(USE_QT_LIBRARIES
    Core
    Gui
    Widgets)

FIND_PACKAGE(Qt6 QUIET COMPONENTS ${USE_QT_LIBRARIES})

if (Qt6_FOUND)
    MESSAGE("-- Qt version is ${Qt6_VERSION}")

    SET(QT_UI_LIB Qt6::Gui)

    SET(QT_ALL_LIBS
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
else()
    FIND_PACKAGE(Qt5 REQUIRED ${USE_QT_LIBRARIES})
    MESSAGE("-- Qt version is ${Qt5_VERSION}")
    SET(QT_UI_LIB Qt5::Gui)

    SET(QT_ALL_LIBS
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)

    macro(QT_RESOURCES FIRST SECOND)
        QT5_ADD_RESOURCES(${FIRST} ${SECOND})
    endmacro()

    macro(QT_WRAP_UIS)
        QT5_WRAP_UI(UI_HEADERS
                   ${UIS})
    endmacro()
endif()


if (NOT BUILD_DATE)
    STRING(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    STRING(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
endif()

if (NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
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

if (APPLE)
    FIND_LIBRARY(APPKIT AppKit)
    SET(OS_LIBS ${APPKIT})

    SET(MACOSX_BUNDLE_EXECUTABLE_NAME ${APPLICATION_NAME})
    SET(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    SET(MACOSX_BUNDLE_BUNDLE_NAME ${APPLICATION_NAME})
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

    ADD_EXECUTABLE(${APPLICATION_NAME}
        MACOSX_BUNDLE
        ../../src/resource/openclipart/vookiimageviewericon.icns
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})

    GET_FILENAME_COMPONENT(PLIST_IN_ABSOLUTE_PATH mac/support/MacOSXBundleInfo.plist.in ABSOLUTE)
    SET_TARGET_PROPERTIES(${APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_IN_ABSOLUTE_PATH})

elseif(WIN32)
    SET(SOURCES
        ${SOURCES}
        ../../src/abstraction/win/init.cpp
        ../../src/abstraction/win/darkmode.cpp
    )

    ADD_EXECUTABLE(${APPLICATION_NAME}
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

TARGET_LINK_LIBRARIES(VookiImageViewer ${QT_ALL_LIBS})
TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})

#### Plugins ####
#

# Personal Computer Exchange (pcx)
#
ADD_LIBRARY(vooki_kimg_pcx MODULE ../../src/plugins/kimageformats/pcx.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pcx ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_pcx)

if (Qt5_FOUND)
    if (Qt5_VERSION VERSION_GREATER_EQUAL 5.8.0)
        # Q_FALLTHROUGH was introduced in 5.8.0
        #

        # Gimp (xcf)
        #
        ADD_LIBRARY(vooki_kimg_xcf MODULE ../../src/plugins/kimageformats/xcf.cpp)
        TARGET_LINK_LIBRARIES(vooki_kimg_xcf ${QT_UI_LIB})
        SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_xcf)

        # Targa (tga): supports more formats than Qt's version
        #
        ADD_LIBRARY(vooki_kimg_tga MODULE ../../src/plugins/kimageformats/tga.cpp)
        TARGET_LINK_LIBRARIES(vooki_kimg_tga ${QT_UI_LIB})
        SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_tga)
    endif()

    if (Qt5_VERSION VERSION_GREATER_EQUAL 5.12.0)
       # Format_RGBA64 was introduced in 5.12.0
       #

       # Photoshop documents (psd)
       #
       ADD_LIBRARY(vooki_kimg_psd MODULE ../../src/plugins/kimageformats/psd.cpp)
       TARGET_LINK_LIBRARIES(vooki_kimg_psd ${QT_UI_LIB})
       SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_psd)
    endif()
endif()

if (Qt6_FOUND)
    # Gimp (xcf)
    #
    ADD_LIBRARY(vooki_kimg_xcf MODULE ../../src/plugins/kimageformats/xcf.cpp)
    TARGET_LINK_LIBRARIES(vooki_kimg_xcf ${QT_UI_LIB})
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_xcf)

    # Targa (tga): supports more formats than Qt's version
    #
    ADD_LIBRARY(vooki_kimg_tga MODULE ../../src/plugins/kimageformats/tga.cpp)
    TARGET_LINK_LIBRARIES(vooki_kimg_tga ${QT_UI_LIB})
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_tga)

    # Photoshop documents (psd)
    #
    ADD_LIBRARY(vooki_kimg_psd MODULE ../../src/plugins/kimageformats/psd.cpp)
    TARGET_LINK_LIBRARIES(vooki_kimg_psd ${QT_UI_LIB})
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_psd)
endif()

# Windows Animated Cursor (ani)
#
ADD_LIBRARY(vooki_kimg_ani MODULE ../../src/plugins/kimageformats/ani.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_ani ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_ani)

# Radiance HDR (hdr)
#
ADD_LIBRARY(vooki_kimg_hdr MODULE ../../src/plugins/kimageformats/hdr.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_hdr ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_hdr)

# Sun Raster (ras)
#
ADD_LIBRARY(vooki_kimg_ras MODULE ../../src/plugins/kimageformats/ras.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_ras ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_ras)

# Softimage PIC (pic)
#
ADD_LIBRARY(vooki_kimg_pic MODULE ../../src/plugins/kimageformats/pic.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pic ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_pic)

# SGI images (rgb, rgba, sgi, bw)
#
ADD_LIBRARY(vooki_kimg_rgb MODULE ../../src/plugins/kimageformats/rgb.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_rgb ${QT_UI_LIB})
SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_kimg_rgb)

# Camera RAW Thumbnails (raf, mos, cr2, erf, dng, mrw, nef, orf, rw2, pef, x3f, srw, x3f, arw)
# Prebuilt library for the MacOS/Windows only, Linux uses a distro package
#
if (APPLE)
    # MacOS
    #
    SET(LIBRAW_LOC "3rdPartyLibs/LibRaw-0.20.2")
    GET_FILENAME_COMPONENT(LIBRAW mac/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L${LIBRAW}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} libraw_r.a)
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_raw_thumb)

    include(mac/MacdeployQt.cmake)
    deployqt()

    SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/../PlugIns/imageformats")

    INSTALL(TARGETS ${IMAGE_PLUGINS}
            LIBRARY DESTINATION ${PLUGINS})
elseif(WIN32)
    # Windows
    #
    SET(LIBRAW_LOC "3rdPartyLibs/LibRaw-0.20.2")
    ADD_DEFINITIONS(-DLIBRAW_NODLL)

    GET_FILENAME_COMPONENT(LIBRAW windows/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")

    ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW}/lib/libraw.lib)
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_raw_thumb)

    GET_FILENAME_COMPONENT(LIBHEIF "windows/3rdPartyLibs/libHEIF-1.9.1" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBHEIF} ${CMAKE_CXX_FLAGS}")

    ADD_LIBRARY(vooki_heic_image MODULE ../../src/plugins/heicimage/heicHandler.cpp ../../src/plugins/heicimage/heicPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_heic_image ${QT_UI_LIB} ${LIBHEIF}/lib/heif.lib)
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_heic_image)

    include(windows/WindeployQt.cmake)
    deployqt()

    SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/imageformats")
    SET_TARGET_PROPERTIES(${IMAGE_PLUGINS}
            PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGINS})

    INSTALL(TARGETS ${IMAGE_PLUGINS}
            LIBRARY DESTINATION ${PLUGINS})
else()
    # Unix-like systems
    #
    MESSAGE("-- ${APPLICATION_NAME} expects its plugins located either in /usr/lib/vookiimageviewer/imageformats or /usr/local/lib/vookiimageviewer/imageformats")
    SET(PLUGINS "${LIB_INSTALL_DIR}/vookiimageviewer/imageformats")
    ADD_DEFINITIONS(-DUNIX_LIKE)

    FIND_LIBRARY(LIBRAW NAMES libraw_r raw_r)
    if (LIBRAW)
        ADD_LIBRARY(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
        TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW})
        SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} vooki_raw_thumb)
    endif()

    INSTALL(TARGETS ${APPLICATION_NAME} ${IMAGE_PLUGINS}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION ${PLUGINS})

    GET_FILENAME_COMPONENT(TOP_LEVEL_ABSOLUTE_PATH ../../ ABSOLUTE)
    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-IMAGES
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-OPENCLIPART
                  ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-PLUGINS-KIMAGEFORMATS
                  ${TOP_LEVEL_ABSOLUTE_PATH}/README.md
            DESTINATION share/doc/vookiimageviewer)

    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/unix/support/vookiimageviewer.desktop
            DESTINATION share/applications)

    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/src/resource/openclipart/vookiimageviewericon.png
            DESTINATION share/pixmaps)

endif()
