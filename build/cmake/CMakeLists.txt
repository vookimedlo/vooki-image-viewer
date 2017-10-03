cmake_minimum_required(VERSION 3.9)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(APPLE)
    find_library(APPKIT AppKit)
    set(OS_LIBS ${APPKIT})
endif(APPLE)

# std::make_unique is part of c++14
#
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(SOURCES
    ../../src/main.cpp
    ../../src/ui/MainWindow.cpp
    ../../src/ui/ImageArea.cpp
    ../../src/model/ImageCatalog.cpp
    ../../src/model/FileSystemSortFilterProxyModel.cpp
    ../../src/util/misc.cpp
    ../../src/ui/support/RecentFileAction.cpp
    ../../src/ui/AboutComponentsDialog.cpp
    ../../src/ui/SettingsDialog.cpp
    ../../src/ui/SettingsShortcutsTableWidget.cpp
    ../../src/ui/support/Settings.cpp
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.cpp
)

if(APPLE)
    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/mac/init.mm
    )
endif(APPLE)

qt5_wrap_ui(UI_HEADERS
    ../../src/ui/forms/MainWindow.ui
    ../../src/ui/forms/AboutComponentsDialog.ui
    ../../src/ui/forms/AboutSupportedFormatsDialog.ui
    ../../src/ui/forms/AboutDialog.ui
    ../../src/ui/forms/SettingsDialog.ui
)

qt5_add_resources(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

if(APPLE)
    set(MACOSX_BUNDLE_EXECUTABLE_NAME VookiImageViewer)
    set(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME VookiImageViewer)
    set_source_files_properties(../../src/resource/openclipart/vookiimageviewericon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif(APPLE)

add_executable(VookiImageViewer
    MACOSX_BUNDLE
    ../../src/resource/openclipart/vookiimageviewericon.icns
    ${SOURCES}
    ${UI_HEADERS}
    ${UI_RESOURCES})

qt5_use_modules(VookiImageViewer
    Core
    Gui
    Widgets)

TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})
