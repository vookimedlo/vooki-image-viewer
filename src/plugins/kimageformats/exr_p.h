/*
* QImageIO Routines to read (and perhaps in the future, write) images
* in the high definition EXR format.
*
* Copyright (c) 2003, Brad Hards <bradh@frogmouth.net>
*
* This library is distributed under the conditions of the GNU LGPL.
*
*/

#ifndef KIMG_EXR_H
#define KIMG_EXR_H

#include <QImageIOPlugin>

class EXRHandler : public QImageIOHandler
{
public:
    EXRHandler();

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *outImage) Q_DECL_OVERRIDE;

    static bool canRead(QIODevice *device);
};

class EXRPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "exr.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_OVERRIDE;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const Q_DECL_OVERRIDE;
};

#endif // KIMG_EXR_H
