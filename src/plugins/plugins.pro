load(qt_parts)

QT_FOR_CONFIG += gui
QT       += core gui widgets printsupport

TARGET  = kimg_eps

HEADERS += eps_p.h

SOURCES += eps.cpp


PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = EPSPlugin
load(qt_plugin)
