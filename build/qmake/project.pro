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
    ../../src/model/ImageCatalog.cpp \
    ../../src/model/FileSystemSortFilterProxyModel.cpp \
    ../../src/ui/AboutComponentsDialog.cpp \
    ../../src/ui/FileSystemTreeView.cpp \
    ../../src/ui/MainWindow.cpp \
    ../../src/ui/SettingsDialog.cpp \
    ../../src/ui/SettingsShortcutsTableWidget.cpp \
    ../../src/ui/support/RecentFileAction.cpp \
    ../../src/ui/support/Settings.cpp \
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.cpp \
    ../../src/util/misc.cpp \
    ../../src/ui/ImageAreaWidget.cpp

HEADERS  += \
    ../../src/abstraction/init.h \
    ../../src/model/ImageCatalog.h \
    ../../src/model/FileSystemSortFilterProxyModel.h \
    ../../src/ui/AboutComponentsDialog.h \
    ../../src/ui/FileSystemTreeView.h \
    ../../src/ui/MainWindow.h \
    ../../src/ui/SettingsDialog.h \
    ../../src/ui/SettingsShortcutsTableWidget.h \
    ../../src/ui/support/RecentFileAction.h \
    ../../src/ui/support/Settings.h \
    ../../src/ui/support/SettingsShortcutsTableWidgetItem.h \
    ../../src/ui/support/SettingsStrings.h \
    ../../src/util/compiler.h \
    ../../src/util/misc.h \
    ../../src/util/RotatingIndex.h \
    ../../src/util/version.h \
    ../../src/ui/ImageAreaWidget.h

FORMS += \
    ../../src/ui/forms/AboutComponentsDialog.ui \
    ../../src/ui/forms/AboutDialog.ui \
    ../../src/ui/forms/AboutSupportedFormatsDialog.ui \
    ../../src/ui/forms/MainWindow.ui \
    ../../src/ui/forms/SettingsDialog.ui

# OS specific
#
mac: OBJECTIVE_SOURCES = ../../src/abstraction/mac/init.mm
mac: LIBS += -framework AppKit
mac: ICON = ../../src/resource/openclipart/vookiimageviewericon.icns
