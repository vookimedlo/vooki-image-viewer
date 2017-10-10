cmake_minimum_required(VERSION 3.9)

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

if(NOT BUILD_DATE)
    string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
endif()

add_definitions(
    -DBUILD_DATE=\"${BUILD_DATE}\"
)

set(SOURCES
    ../../src/main.cpp
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

qt5_add_resources(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

if(APPLE)
    find_library(APPKIT AppKit)
    set(OS_LIBS ${APPKIT})

    set(MACOSX_BUNDLE_EXECUTABLE_NAME VookiImageViewer)
    set(MACOSX_BUNDLE_ICON_FILE vookiimageviewericon.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME VookiImageViewer)
    set(MACOSX_BUNDLE_COPYRIGHT "(C) 2017 Michal Duda")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${BUILD_DATE})
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
elseif(WIN32)
    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/win/init.cpp
    )

    add_executable(VookiImageViewer
        WIN32
        windows/application.rc
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
