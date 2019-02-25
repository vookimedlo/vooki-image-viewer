// kimgio module for SGI images
//
// Copyright (C) 2004  Melchior FRANZ  <mfranz@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the Lesser GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.

#ifndef KIMG_RGB_P_H
#define KIMG_RGB_P_H

#include <QImageIOPlugin>

class RGBHandler : public QImageIOHandler
{
public:
    RGBHandler();

    bool canRead() const override;
    bool read(QImage *image) override;
    bool write(const QImage &image) override;

    static bool canRead(QIODevice *device);
};

class RGBPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "rgb.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const override;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const override;
};

#endif // KIMG_RGB_P_H

