/*
* QImageIO Routines to read/write EPS images.
* copyright (c) 1998 Dirk Schoenberger <dirk.schoenberger@freenet.de>
*
* This library is distributed under the conditions of the GNU LGPL.
*/
#ifndef KIMG_EPS_H
#define KIMG_EPS_H

#include <QImageIOPlugin>
#include <QLoggingCategory>

class EPSHandler : public QImageIOHandler
{
public:
    EPSHandler();

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *image) Q_DECL_OVERRIDE;
    bool write(const QImage &image) Q_DECL_OVERRIDE;

    static bool canRead(QIODevice *device);
};

class EPSPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "eps.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_OVERRIDE;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const Q_DECL_OVERRIDE;
};

Q_DECLARE_LOGGING_CATEGORY(EPSPLUGIN)

#endif // KIMG_EPS_H

