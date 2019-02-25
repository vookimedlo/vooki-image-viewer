/*
* QImageIO Routines to read/write EPS images.
* copyright (c) 1998 Dirk Schoenberger <dirk.schoenberger@freenet.de>
*
* This library is distributed under the conditions of the GNU LGPL.
*/
#ifndef KIMG_EPS_P_H
#define KIMG_EPS_P_H

#include <QImageIOPlugin>
#include <QLoggingCategory>

class EPSHandler : public QImageIOHandler
{
public:
    EPSHandler();

    bool canRead() const override;
    bool read(QImage *image) override;
    bool write(const QImage &image) override;

    static bool canRead(QIODevice *device);
};

class EPSPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "eps.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const override;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const override;
};

Q_DECLARE_LOGGING_CATEGORY(EPSPLUGIN)

#endif // KIMG_EPS_P_H

