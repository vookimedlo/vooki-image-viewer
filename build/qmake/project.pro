#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T20:42:10
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = VookiImageViewer
TEMPLATE = app

# std::make_unique is part of c++14
#
CONFIG += c++14

RESOURCES += \
    ../../src/resource/vookiimageviewer.qrc

SOURCES += \
    ../../src/main.cpp \
    ../../src/ui/MainWindow.cpp \
    ../../src/ui/ImageArea.cpp \
    ../../src/model/ImageCatalog.cpp \
    ../../src/util/misc.cpp \
    ../../src/ui/support/RecentFileAction.cpp \
    ../../src/ui/AboutComponentsDialog.cpp \
    ../../src/ui/SettingsDialog.cpp \
    ../../src/ui/SettingsShortcutsTableWidget.cpp \
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.cpp \
    ../../src/model/FileSystemSortFilterProxyModel.cpp \
    ../../src/ui/support/Settings.cpp


HEADERS  += \
    ../../src/abstraction/init.h \
    ../../src/ui/MainWindow.h \
    ../../src/ui/ImageArea.h \
    ../../src/model/ImageCatalog.h \
    ../../src/util/compiler.h \
    ../../src/util/misc.h \
    ../../src/util/RotatingIndex.h \
    ../../src/ui/support/RecentFileAction.h \
    ../../src/ui/AboutComponentsDialog.h \
    ../../src/ui/SettingsDialog.h \
    ../../src/ui/SettingsShortcutsTableWidget.h \
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.h \
    ../../src/model/FileSystemSortFilterProxyModel.h \
    ../../src/ui/support/Settings.h \
    ../../src/ui/support/SettingsStrings.h \
    ../../src/util/version.h

FORMS += \
    ../../src/ui/forms/MainWindow.ui \
    ../../src/ui/forms/AboutComponentsDialog.ui \
    ../../src/ui/forms/AboutSupportedFormatsDialog.ui \
    ../../src/ui/forms/AboutDialog.ui \
    ../../src/ui/forms/SettingsDialog.ui

# OS specific
#
mac: OBJECTIVE_SOURCES = ../../src/abstraction/mac/init.mm
mac: LIBS += -framework AppKit
mac: ICON = ../../src/resource/openclipart/vookiimageviewericon.icns
