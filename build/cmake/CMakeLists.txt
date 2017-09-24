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
    ../../src/util/misc.cpp
    ../../src/ui/support/RecentFileAction.cpp
    ../../src/ui/AboutComponentsDialog.cpp
)

if(APPLE)
    set(SOURCES
        ${SOURCES}
        ../../src/abstraction/mac/init.mm
    )
endif(APPLE)

qt5_wrap_ui(UI_HEADERS
    ../../src/ui/forms/MainWindow.ui
    ../../src/ui/forms/About.ui
    ../../src/ui/forms/AboutComponentsDialog.ui)

qt5_add_resources(UI_RESOURCES ../../src/resource/vookiimageviewer.qrc)

add_executable(VookiImageViewer
    MACOSX_BUNDLE
    ${SOURCES}
    ${UI_HEADERS}
    ${UI_RESOURCES})

qt5_use_modules(VookiImageViewer
    Core
    Gui
    Widgets)

TARGET_LINK_LIBRARIES(VookiImageViewer ${OS_LIBS})
