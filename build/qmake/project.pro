#-------------------------------------------------
#
# Project created by QtCreator 2017-01-02T20:42:10
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = VookiImageViever
TEMPLATE = app

# std::make_unique is part of c++14
#
CONFIG += c++14

SOURCES += \
    ../../src/main.cpp \
    ../../src/ui/MainWindow.cpp \
    ../../src/ui/ImageArea.cpp


HEADERS  += \
    ../../src/ui/MainWindow.h \
    ../../src/ui/ImageArea.h

FORMS += \
    ../../src/ui/forms/MainWindow.ui

