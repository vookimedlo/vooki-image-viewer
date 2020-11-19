/*
    Photoshop File Format support for QImage.

    SPDX-FileCopyrightText: 2003 Ignacio Castaño <castano@ludicon.com>
    SPDX-FileCopyrightText: 2015 Alex Merry <alex.merry@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

/*
 * This code is based on Thacher Ulrich PSD loading code released
 * into the public domain. See: http://tulrich.com/geekstuff/
 */

/*
 * Documentation on this file format is available at
 * http://www.adobe.com/devnet-apps/photoshop/fileformatashtml/
 */

#include "psd_p.h"

#include "rle_p.h"

#include <QDataStream>
#include <QDebug>
#include <QImage>

typedef quint32 uint;
typedef quint16 ushort;
typedef quint8 uchar;

namespace   // Private.
{

enum ColorMode {
    CM_BITMAP = 0,
    CM_GRAYSCALE = 1,
    CM_INDEXED = 2,
    CM_RGB = 3,
    CM_CMYK = 4,
    CM_MULTICHANNEL = 7,
    CM_DUOTONE = 8,
    CM_LABCOLOR = 9
};

struct PSDHeader {
    uint signature;
    ushort version;
    uchar reserved[6];
    ushort channel_count;
    uint height;
    uint width;
    ushort depth;
    ushort color_mode;
};

static QDataStream &operator>> (QDataStream &s, PSDHeader &header)
{
    s >> header.signature;
    s >> header.version;
    for (int i = 0; i < 6; i++) {
        s >> header.reserved[i];
    }
    s >> header.channel_count;
    s >> header.height;
    s >> header.width;
    s >> header.depth;
    s >> header.color_mode;
    return s;
}

// Check that the header is a valid PSD.
static bool IsValid(const PSDHeader &header)
{
    if (header.signature != 0x38425053) {    // '8BPS'
        return false;
    }
    return true;
}

// Check that the header is supported.
static bool IsSupported(const PSDHeader &header)
{
    if (header.version != 1) {
        return false;
    }
    if (header.channel_count > 16) {
        return false;
    }
    if (header.depth != 8 && header.depth != 16) {
        return false;
    }
    if (header.color_mode != CM_RGB) {
        return false;
    }
    return true;
}

static void skip_section(QDataStream &s)
{
    quint32 section_length;
    // Skip mode data.
    s >> section_length;
    s.skipRawData(section_length);
}

template <class Trait>
static Trait readPixel(QDataStream &stream) {
    Trait pixel;
    stream >> pixel;
    return pixel;
}

static QRgb updateRed(QRgb oldPixel, quint8 redPixel) {
    return qRgba(redPixel, qGreen(oldPixel), qBlue(oldPixel), qAlpha(oldPixel));
}
static QRgb updateGreen(QRgb oldPixel, quint8 greenPixel) {
    return qRgba(qRed(oldPixel), greenPixel, qBlue(oldPixel), qAlpha(oldPixel));
}
static QRgb updateBlue(QRgb oldPixel, quint8 bluePixel) {
    return qRgba(qRed(oldPixel), qGreen(oldPixel), bluePixel, qAlpha(oldPixel));
}
static QRgb updateAlpha(QRgb oldPixel, quint8 alphaPixel) {
    return qRgba(qRed(oldPixel), qGreen(oldPixel), qBlue(oldPixel), alphaPixel);
}
typedef QRgb(*channelUpdater)(QRgb,quint8);

// Load the PSD image.
static bool LoadPSD(QDataStream &stream, const PSDHeader &header, QImage &img)
{
    // Mode data
    skip_section(stream);

    // Image resources
    skip_section(stream);

    // Reserved data
    skip_section(stream);

    // Find out if the data is compressed.
    // Known values:
    //   0: no compression
    //   1: RLE compressed
    quint16 compression;
    stream >> compression;

    if (compression > 1) {
        qDebug() << "Unknown compression type";
        return false;
    }

    quint32 channel_num = header.channel_count;

    QImage::Format fmt = header.depth == 8 ? QImage::Format_RGB32
                                           : QImage::Format_RGBX64;
    // Clear the image.
    if (channel_num >= 4) {
        // Enable alpha.
        fmt = header.depth == 8 ? QImage::Format_ARGB32
                                : QImage::Format_RGBA64;

        // Ignore the other channels.
        channel_num = 4;
    }

    img = QImage(header.width, header.height, fmt);
    if (img.isNull()) {
        qWarning() << "Failed to allocate image, invalid dimensions?" << QSize(header.width, header.height);
        return false;
    }
    img.fill(qRgb(0,0,0));

    const quint32 pixel_count = header.height * header.width;
    const quint32 channel_size = pixel_count * header.depth / 8;

    // Verify this, as this is used to write into the memory of the QImage
    if (pixel_count > img.sizeInBytes() / (header.depth == 8 ? sizeof(QRgb) : sizeof(QRgba64))) {
        qWarning() << "Invalid pixel count!" << pixel_count << "bytes available:" << img.sizeInBytes();
        return false;
    }

    QRgb *image_data = reinterpret_cast<QRgb*>(img.bits());

    if (!image_data) {
        return false;
    }

    static const channelUpdater updaters[4] = {
        updateRed,
        updateGreen,
        updateBlue,
        updateAlpha
    };

    typedef QRgba64(*channelUpdater16)(QRgba64, quint16);
    static const channelUpdater16 updaters64[4] = {
        [](QRgba64 oldPixel, quint16 redPixel)  {return qRgba64((oldPixel & ~(0xFFFFull <<  0)) | (quint64(  redPixel) <<  0));},
        [](QRgba64 oldPixel, quint16 greenPixel){return qRgba64((oldPixel & ~(0xFFFFull << 16)) | (quint64(greenPixel) << 16));},
        [](QRgba64 oldPixel, quint16 bluePixel) {return qRgba64((oldPixel & ~(0xFFFFull << 32)) | (quint64( bluePixel) << 32));},
        [](QRgba64 oldPixel, quint16 alphaPixel){return qRgba64((oldPixel & ~(0xFFFFull << 48)) | (quint64(alphaPixel) << 48));}
    };

    if (compression) {
        // Skip row lengths.
        int skip_count = header.height * header.channel_count * sizeof(quint16);
        if (stream.skipRawData(skip_count) != skip_count) {
            return false;
        }

        for (unsigned short channel = 0; channel < channel_num; channel++) {
            bool success = false;
            if (header.depth == 8) {
                success = decodeRLEData(RLEVariant::PackBits, stream,
                                         image_data, channel_size,
                                         &readPixel<quint8>, updaters[channel]);
            } else if (header.depth == 16) {
                QRgba64 *image_data = reinterpret_cast<QRgba64*>(img.bits());
                success = decodeRLEData(RLEVariant::PackBits16, stream,
                                         image_data, channel_size,
                                         &readPixel<quint8>, updaters64[channel]);
            }

            if (!success) {
                qDebug() << "decodeRLEData on channel" << channel << "failed";
                return false;
            }
        }
    } else {
        for (unsigned short channel = 0; channel < channel_num; channel++) {
            if (header.depth == 8) {
                for (unsigned i = 0; i < pixel_count; ++i) {
                    image_data[i] = updaters[channel](image_data[i], readPixel<quint8>(stream));
                }
            } else if (header.depth == 16) {
                QRgba64 *image_data = reinterpret_cast<QRgba64*>(img.bits());
                for (unsigned i = 0; i < pixel_count; ++i) {
                    image_data[i] = updaters64[channel](image_data[i], readPixel<quint16>(stream));
                }
            }
            // make sure we didn't try to read past the end of the stream
            if (stream.status() != QDataStream::Ok) {
                qDebug() << "DataStream status was" << stream.status();
                return false;
            }
        }
    }

    return true;
}

} // Private

PSDHandler::PSDHandler()
{
}

bool PSDHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("psd");
        return true;
    }
    return false;
}

bool PSDHandler::read(QImage *image)
{
    QDataStream s(device());
    s.setByteOrder(QDataStream::BigEndian);

    PSDHeader header;
    s >> header;

    // Check image file format.
    if (s.atEnd() || !IsValid(header)) {
//         qDebug() << "This PSD file is not valid.";
        return false;
    }

    // Check if it's a supported format.
    if (!IsSupported(header)) {
//         qDebug() << "This PSD file is not supported.";
        return false;
    }

    QImage img;
    if (!LoadPSD(s, header, img)) {
//         qDebug() << "Error loading PSD file.";
        return false;
    }

    *image = img;
    return true;
}

bool PSDHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("PSDHandler::canRead() called with no device");
        return false;
    }

    qint64 oldPos = device->pos();

    char head[4];
    qint64 readBytes = device->read(head, sizeof(head));
    if (readBytes < 0) {
        qWarning() << "Read failed" << device->errorString();
        return false;
    }

    if (readBytes != sizeof(head)) {
        if (device->isSequential()) {
            while (readBytes > 0) {
                device->ungetChar(head[readBytes-- - 1]);
            }
        } else {
            device->seek(oldPos);
        }
        return false;
    }

    if (device->isSequential()) {
        while (readBytes > 0) {
            device->ungetChar(head[readBytes-- - 1]);
        }
    } else {
        device->seek(oldPos);
    }

    return qstrncmp(head, "8BPS", 4) == 0;
}

QImageIOPlugin::Capabilities PSDPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "psd") {
        return Capabilities(CanRead);
    }
    if (!format.isEmpty()) {
        return {};
    }
    if (!device->isOpen()) {
        return {};
    }

    Capabilities cap;
    if (device->isReadable() && PSDHandler::canRead(device)) {
        cap |= CanRead;
    }
    return cap;
}

QImageIOHandler *PSDPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new PSDHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
