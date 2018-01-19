cmake_minimum_required(VERSION 3.2)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

# std::make_unique is part of c++14
#
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(NOT BUILD_DATE)
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
    string(TIMESTAMP BUILD_DATE_DOTTED "%Y.%m.%d")
endif()

add_definitions(
    -DBUILD_DATE=\"${BUILD_DATE_DOTTED}\"
)

set(SOURCES
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

qt5_wrap_ui(UI_HEADERS
    ../../src/ui/forms/AboutComponentsDialog.ui
    ../../src/ui/forms/AboutDialog.ui
    ../../src/ui/forms/AboutSupportedFormatsDialog.ui
    ../../src/ui/forms/MainWindow.ui
    ../../src/ui/forms/SettingsDialog.ui
)

# Satisfy new Policy CMP0071
#
set_property(SOURCE "ui_AboutComponentsDialog.h" PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE "ui_AboutDialog.h" PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE "ui_AboutSupportedFormatsDialog.h" PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE "ui_MainWindow.h" PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE "ui_SettingsDialog.h" PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE "qrc_vookiimageviewer.cpp" PROPERTY SKIP_AUTOMOC ON)

qt5_add_resources(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

if(APPLE)
    find_library(APPKIT AppKit)
    set(OS_LIBS ${APPKIT})

    set(MACOSX_BUNDLE_EXECUTABLE_NAME VookiImageViewer)
    set(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME VookiImageViewer)
    set(MACOSX_BUNDLE_COPYRIGHT "(C) 2017 Michal Duda")
    set(MACOSX_BUNDLE_INFO_STRING "Lightweight image viewer")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${BUILD_DATE_DOTTED})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "cz.VookiImageViewer")

    set_source_files_properties(../../src/resource/openclipart/vookiimageviewericon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/mac/init.mm
    )

    add_executable(VookiImageViewer
        MACOSX_BUNDLE
        ../../src/resource/openclipart/vookiimageviewericon.icns
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})

    set_target_properties(VookiImageViewer PROPERTIES MACOSX_BUNDLE_INFO_PLIST ../support/MacOSXBundleInfo.plist.in)

elseif(WIN32)
    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/win/init.cpp
    )

    add_executable(VookiImageViewer
        WIN32
        windows/support/application.rc
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})
else()
    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/unix/init.cpp
    )

    add_executable(VookiImageViewer
        ${SOURCES}
        ${UI_HEADERS}
        ${UI_RESOURCES})
endif()

qt5_use_modules(VookiImageViewer
    Core
    Gui
    Widgets)

TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})


#### Plugins ####
#

# Photoshop documents (psd)
#
add_library(vooki_kimg_psd MODULE ../../src/plugins/kimageformats/psd.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_psd Qt5::Gui)

# Gimp (xcf)
#
add_library(vooki_kimg_xcf MODULE ../../src/plugins/kimageformats/xcf.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_xcf Qt5::Gui)

# Personal Computer Exchange (pcx)
#
add_library(vooki_kimg_pcx MODULE ../../src/plugins/kimageformats/pcx.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pcx Qt5::Gui)

# Targa (tga): supports more formats than Qt's version
#
add_library(vooki_kimg_tga MODULE ../../src/plugins/kimageformats/tga.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_tga Qt5::Gui)

# Sun Raster (ras)
#
add_library(vooki_kimg_ras MODULE ../../src/plugins/kimageformats/ras.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_ras Qt5::Gui)

# Softimage PIC (pic)
#
add_library(vooki_kimg_pic MODULE ../../src/plugins/kimageformats/pic.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_pic Qt5::Gui)

# SGI images (rgb, rgba, sgi, bw)
#
add_library(vooki_kimg_rgb MODULE ../../src/plugins/kimageformats/rgb.cpp)
TARGET_LINK_LIBRARIES(vooki_kimg_rgb Qt5::Gui)

# Camera RAW Thumbnails (raf, mos, cr2, erf, dng, mrw, nef, orf, rw2, pef, x3f, srw, x3f, arw)
#
if(APPLE)
    SET(LIBRAW "3rdPartyLibs/LibRaw-0.18.7")
    SET(CMAKE_CXX_FLAGS "-I../${LIBRAW} ${CMAKE_CXX_FLAGS}")
    SET(CMAKE_MODULE_LINKER_FLAGS "-L../${LIBRAW}/lib ${CMAKE_MODULE_LINKER_FLAGS}")

    add_library(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui libraw_r.a)
elseif(WIN32)
    add_definitions(-DLIBRAW_NODLL)

    SET(LIBRAW "3rdPartyLibs/LibRaw-0.18.7")
    SET(CMAKE_CXX_FLAGS "-I../${LIBRAW} ${CMAKE_CXX_FLAGS}")

    add_library(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui ../${LIBRAW}/lib/libraw)
else()
    find_library(LIBRAW NAMES libraw_r raw_r)
    add_library(vooki_raw_thumb MODULE ../../src/plugins/rawthumb/rawThumbHandler.cpp ../../src/plugins/rawthumb/rawThumbPlugin.cpp)
    TARGET_LINK_LIBRARIES(vooki_raw_thumb Qt5::Gui ${LIBRAW})
endif()
