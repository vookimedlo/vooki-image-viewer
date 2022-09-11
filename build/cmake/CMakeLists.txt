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

PROJECT(vookiimageviewer)

CMAKE_MINIMUM_REQUIRED(VERSION 3.22)

SET(APPLICATION_NAME VookiImageViewer)
MESSAGE("-- ${APPLICATION_NAME} - https://vookiimageviewer.cz")

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules ${CMAKE_MODULE_PATH})

if (NOT VERSION)
    include(GetGitRevisionDescription)
    get_git_head_revision(HEAD_REFS HEAD_HASH "ALLOW_LOOKING_ABOVE_CMAKE_SOURCE_DIR")
    git_describe_working_tree(HASH_TAG --tags --always --exact-match "${HEAD_HASH}")

    SET(VERSION_REGEXP "[0-9][0-9][0-9][0-9]\\.[0-9][0-9]\\.[0-9][0-9]")
    STRING(REGEX REPLACE "^v(${VERSION_REGEXP})$" "\\1" VERSION "${HASH_TAG}")
    STRING(REGEX MATCHALL "^${VERSION_REGEXP}$" VERSION_MATCH "${VERSION}")
    if (NOT VERSION_MATCH)
        STRING(REGEX REPLACE "^(......).*$" "\\1" VERSION_HASH "${HEAD_HASH}")
        STRING(REGEX MATCHALL "^refs/heads/([^/]+)$" VERSION_MATCH "${HEAD_REFS}")
        if (VERSION_MATCH AND CMAKE_MATCH_COUNT EQUAL 1)
            STRING(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
            SET(VERSION "${BUILD_DATE_DOTTED}-${CMAKE_MATCH_1}-${VERSION_HASH}")
        elseif()
            SET(VERSION "${BUILD_DATE_DOTTED}-${VERSION_HASH}")
        endif ()
    endif()
endif()

MESSAGE("-- Application version is ${VERSION}")
CONFIGURE_FILE(../../src/util/version.h.in "${CMAKE_BINARY_DIR}/version.h" @ONLY)

if (APPLE)
    SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "")
    MESSAGE("-- Requested architectures are x86_64 & arm64")
endif()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
SET(CMAKE_MACOSX_RPATH ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

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

if (NOT LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR lib)
endif()

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
    SET(MACOSX_BUNDLE_COPYRIGHT "Michal Duda")
    SET(MACOSX_BUNDLE_INFO_STRING "Lightweight image viewer")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION})
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

    GET_FILENAME_COMPONENT(PLIST_IN_ABSOLUTE_PATH platform/mac/support/MacOSXBundleInfo.plist.in ABSOLUTE)
    SET_TARGET_PROPERTIES(${APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PLIST_IN_ABSOLUTE_PATH})

elseif(WIN32)
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

function(ADD_IMAGE_PLUGIN name source)
    ADD_LIBRARY(${name} MODULE ${source} ${ARGN})
    TARGET_LINK_LIBRARIES(${name} ${QT_UI_LIB})
    SET(IMAGE_PLUGINS ${IMAGE_PLUGINS} ${name} PARENT_SCOPE)
endfunction()


# Personal Computer Exchange (pcx)
#
ADD_IMAGE_PLUGIN(vooki_kimg_pcx ../../src/plugins/kimageformats/pcx.cpp)

if (Qt5_FOUND)
    if (Qt5_VERSION VERSION_GREATER_EQUAL 5.8.0)
        # Q_FALLTHROUGH was introduced in 5.8.0
        #

        # Gimp (xcf)
        #
        ADD_IMAGE_PLUGIN(vooki_kimg_xcf ../../src/plugins/kimageformats/xcf.cpp)

        # Targa (tga): supports more formats than Qt's version
        #
        ADD_IMAGE_PLUGIN(vooki_kimg_tga ../../src/plugins/kimageformats/tga.cpp)
    endif()

    if (Qt5_VERSION VERSION_GREATER_EQUAL 5.12.0)
       # Format_RGBA64 was introduced in 5.12.0
       #

       # Photoshop documents (psd)
       #
       ADD_IMAGE_PLUGIN(vooki_kimg_psd ../../src/plugins/kimageformats/psd.cpp)
    endif()
endif()

if (Qt6_FOUND)
    # Gimp (xcf)
    #
    ADD_IMAGE_PLUGIN(vooki_kimg_xcf ../../src/plugins/kimageformats/xcf.cpp)

    # Targa (tga): supports more formats than Qt's version
    #
    ADD_IMAGE_PLUGIN(vooki_kimg_tga ../../src/plugins/kimageformats/tga.cpp)

    # Photoshop documents (psd)
    #
    ADD_IMAGE_PLUGIN(vooki_kimg_psd ../../src/plugins/kimageformats/psd.cpp)
endif()

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
    SET(LIBRAW_LOC "3rdPartyLibs/LibRaw-0.20.2")
    GET_FILENAME_COMPONENT(LIBRAW platform/mac/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L${LIBRAW}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_raw_thumb ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} libraw_r.a)

    include(platform/mac/MacdeployQt.cmake)
    deployqt()

    SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/../PlugIns/imageformats")

    INSTALL(TARGETS ${IMAGE_PLUGINS}
            LIBRARY DESTINATION ${PLUGINS})
elseif(WIN32)
    # Windows
    #
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

    SET(LIBRAW_LOC "3rdPartyLibs/LibRaw-0.20.2")
    ADD_DEFINITIONS(-DLIBRAW_NODLL)

    GET_FILENAME_COMPONENT(LIBRAW platform/windows/${LIBRAW_LOC} ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBRAW} ${CMAKE_CXX_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_raw_thumb ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW}/lib/libraw.lib)

    GET_FILENAME_COMPONENT(LIBHEIF "platform/windows/3rdPartyLibs/LibHEIF-1.13.0" ABSOLUTE)
    SET(CMAKE_CXX_FLAGS "-I${LIBHEIF} ${CMAKE_CXX_FLAGS}")

    ADD_IMAGE_PLUGIN(vooki_kimg_heif ../../src/plugins/kimageformats/heif.cpp)
    TARGET_LINK_LIBRARIES(vooki_kimg_heif ${QT_UI_LIB} ${LIBHEIF}/lib/heif.lib)

    SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/imageformats")
    SET_TARGET_PROPERTIES(${IMAGE_PLUGINS}
            PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PLUGINS})

    ADD_CUSTOM_TARGET(copy-heif-deps)
    GET_FILENAME_COMPONENT(LIBDE265 "platform/windows/3rdPartyLibs/LibDe265-1.0.8/lib/libde265.dll" ABSOLUTE)
    GET_FILENAME_COMPONENT(LIBHEIF "platform/windows/3rdPartyLibs/LibHEIF-1.13.0/lib/heif.dll" ABSOLUTE)
    ADD_CUSTOM_COMMAND(TARGET copy-heif-deps POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy ${LIBDE265} ${LIBHEIF} $<TARGET_FILE_DIR:${APPLICATION_NAME}>)
    ADD_DEPENDENCIES(vooki_kimg_heif copy-heif-deps)
    ADD_DEPENDENCIES(copy-heif-deps ${APPLICATION_NAME})

    include(platform/windows/WindeployQt.cmake)
    deployqt()

    INCLUDE(platform/windows/CPack.cmake)
else()
    # Unix-like systems
    #
    MESSAGE("-- ${APPLICATION_NAME} expects its plugins located either in /usr/lib/vookiimageviewer/imageformats or /usr/local/lib/vookiimageviewer/imageformats")
    SET(PLUGINS "${LIB_INSTALL_DIR}/vookiimageviewer/imageformats")
    ADD_DEFINITIONS(-DUNIX_LIKE)

    FIND_LIBRARY(LIBRAW NAMES libraw_r raw_r)
    if (LIBRAW)
        ADD_IMAGE_PLUGIN(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
        TARGET_LINK_LIBRARIES(vooki_raw_thumb ${QT_UI_LIB} ${LIBRAW})
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

    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/vookiimageviewer.desktop
            DESTINATION share/applications)

    INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/src/resource/openclipart/vookiimageviewericon.png
            DESTINATION share/pixmaps)

endif()
