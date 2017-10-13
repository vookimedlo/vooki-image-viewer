/* This file is part of the KDE project
   Copyright (C) 2002-2003 Nadeem Hasan <nhasan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef KIMG_PCX_H
#define KIMG_PCX_H

#include <QImageIOPlugin>

class PCXHandler : public QImageIOHandler
{
public:
    PCXHandler();

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *image) Q_DECL_OVERRIDE;
    bool write(const QImage &image) Q_DECL_OVERRIDE;

    static bool canRead(QIODevice *device);
};

class PCXPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "pcx.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_OVERRIDE;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const Q_DECL_OVERRIDE;
};

#endif // KIMG_PCX_H
