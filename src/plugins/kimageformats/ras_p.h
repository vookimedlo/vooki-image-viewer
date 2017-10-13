/* This file is part of the KDE project
   Copyright (C) 2003 Dominik Seichter <domseichter@web.de>
   Copyright (C) 2010 Troy Unrau <troy@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef KIMG_RAS_H
#define KIMG_RAS_H

#include <QImageIOPlugin>

class RASHandler : public QImageIOHandler
{
public:
    RASHandler();

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *image) Q_DECL_OVERRIDE;

    static bool canRead(QIODevice *device);
};

class RASPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "ras.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_OVERRIDE;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const Q_DECL_OVERRIDE;
};

#endif // KIMG_RAS_H

