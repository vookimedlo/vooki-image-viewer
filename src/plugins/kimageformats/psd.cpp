/*
 * Photoshop File Format support for QImage.
 *
 * Copyright 2003 Ignacio Castaño <castano@ludicon.com>
 * Copyright 2015 Alex Merry <alex.merry@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    if (header.depth != 8) {
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

static quint8 readPixel(QDataStream &stream) {
    quint8 pixel;
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

    QImage::Format fmt = QImage::Format_RGB32;
    // Clear the image.
    if (channel_num >= 4) {
        // Enable alpha.
        fmt = QImage::Format_ARGB32;

        // Ignore the other channels.
        channel_num = 4;
    }
    img = QImage(header.width, header.height, fmt);
    img.fill(qRgb(0,0,0));

    const quint32 pixel_count = header.height * header.width;

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

    if (compression) {
        // Skip row lengths.
        int skip_count = header.height * header.channel_count * sizeof(quint16);
        if (stream.skipRawData(skip_count) != skip_count) {
            return false;
        }

        for (unsigned short channel = 0; channel < channel_num; channel++) {
            bool success = decodeRLEData(RLEVariant::PackBits, stream,
			                 image_data, pixel_count,
					 &readPixel, updaters[channel]);
            if (!success) {
                qDebug() << "decodeRLEData on channel" << channel << "failed";
                return false;
            }
        }
    } else {
        for (unsigned short channel = 0; channel < channel_num; channel++) {
            for (unsigned i = 0; i < pixel_count; ++i) {
                image_data[i] = updaters[channel](image_data[i], readPixel(stream));
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
