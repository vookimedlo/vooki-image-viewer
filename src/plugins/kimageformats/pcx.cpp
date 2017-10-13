/* This file is part of the KDE project
   Copyright (C) 2002-2005 Nadeem Hasan <nhasan@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License (LGPL) as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "pcx_p.h"

#include <QColor>
#include <QDataStream>
// #include <QDebug>
#include <QImage>


#pragma pack(push,1)
class RGB
{
public:
    quint8 r;
    quint8 g;
    quint8 b;

    static RGB from(const QRgb &color)
    {
        RGB c;
        c.r = qRed(color);
        c.g = qGreen(color);
        c.b = qBlue(color);
        return c;
    }

};

class Palette
{
public:
    void setColor(int i, const QRgb color)
    {
        RGB &c = rgb[ i ];
        c.r = qRed(color);
        c.g = qGreen(color);
        c.b = qBlue(color);
    }

    QRgb color(int i) const
    {
        return qRgb(rgb[ i ].r, rgb[ i ].g, rgb[ i ].b);
    }

    class RGB rgb[ 16 ];
};

class PCXHEADER
{
public:
    PCXHEADER();

    inline int width() const
    {
        return (XMax - XMin) + 1;
    }
    inline int height() const
    {
        return (YMax - YMin) + 1;
    }
    inline bool isCompressed() const
    {
        return (Encoding == 1);
    }

    quint8  Manufacturer;    // Constant Flag, 10 = ZSoft .pcx
    quint8  Version;         // Version information·
    // 0 = Version 2.5 of PC Paintbrush·
    // 2 = Version 2.8 w/palette information·
    // 3 = Version 2.8 w/o palette information·
    // 4 = PC Paintbrush for Windows(Plus for
    //     Windows uses Ver 5)·
    // 5 = Version 3.0 and > of PC Paintbrush
    //     and PC Paintbrush +, includes
    //     Publisher's Paintbrush . Includes
    //     24-bit .PCX files·
    quint8  Encoding;        // 1 = .PCX run length encoding
    quint8  Bpp;             // Number of bits to represent a pixel
    // (per Plane) - 1, 2, 4, or 8·
    quint16 XMin;
    quint16 YMin;
    quint16 XMax;
    quint16 YMax;
    quint16 HDpi;
    quint16 YDpi;
    Palette  ColorMap;
    quint8  Reserved;        // Should be set to 0.
    quint8  NPlanes;         // Number of color planes
    quint16 BytesPerLine;    // Number of bytes to allocate for a scanline
    // plane.  MUST be an EVEN number.  Do NOT
    // calculate from Xmax-Xmin.·
    quint16 PaletteInfo;     // How to interpret palette- 1 = Color/BW,
    // 2 = Grayscale ( ignored in PB IV/ IV + )·
    quint16 HScreenSize;     // Horizontal screen size in pixels. New field
    // found only in PB IV/IV Plus
    quint16 VScreenSize;     // Vertical screen size in pixels. New field
    // found only in PB IV/IV Plus
};

#pragma pack(pop)

static QDataStream &operator>>(QDataStream &s, RGB &rgb)
{
    quint8 r, g, b;

    s >> r >> g >> b;
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;

    return s;
}

static QDataStream &operator>>(QDataStream &s, Palette &pal)
{
    for (int i = 0; i < 16; ++i) {
        s >> pal.rgb[ i ];
    }

    return s;
}

static QDataStream &operator>>(QDataStream &s, PCXHEADER &ph)
{
    quint8 m, ver, enc, bpp;
    s >> m >> ver >> enc >> bpp;
    ph.Manufacturer = m;
    ph.Version = ver;
    ph.Encoding = enc;
    ph.Bpp = bpp;
    quint16 xmin, ymin, xmax, ymax;
    s >> xmin >> ymin >> xmax >> ymax;
    ph.XMin = xmin;
    ph.YMin = ymin;
    ph.XMax = xmax;
    ph.YMax = ymax;
    quint16 hdpi, ydpi;
    s >> hdpi >> ydpi;
    ph.HDpi = hdpi;
    ph.YDpi = ydpi;
    Palette colorMap;
    quint8 res, np;
    s >> colorMap >> res >> np;
    ph.ColorMap = colorMap;
    ph.Reserved = res;
    ph.NPlanes = np;
    quint16 bytesperline;
    s >> bytesperline; ph.BytesPerLine = bytesperline;
    quint16 paletteinfo;
    s >> paletteinfo; ph.PaletteInfo = paletteinfo;
    quint16 hscreensize, vscreensize;
    s >> hscreensize; ph.HScreenSize = hscreensize;
    s >> vscreensize; ph.VScreenSize = vscreensize;

    // Skip the rest of the header
    quint8 byte;
    while (s.device()->pos() < 128) {
        s >> byte;
    }

    return s;
}

static QDataStream &operator<<(QDataStream &s, const RGB &rgb)
{
    s << rgb.r << rgb.g << rgb.b;

    return s;
}

static QDataStream &operator<<(QDataStream &s, const Palette &pal)
{
    for (int i = 0; i < 16; ++i) {
        s << pal.rgb[ i ];
    }

    return s;
}

static QDataStream &operator<<(QDataStream &s, const PCXHEADER &ph)
{
    s << ph.Manufacturer;
    s << ph.Version;
    s << ph.Encoding;
    s << ph.Bpp;
    s << ph.XMin << ph.YMin << ph.XMax << ph.YMax;
    s << ph.HDpi << ph.YDpi;
    s << ph.ColorMap;
    s << ph.Reserved;
    s << ph.NPlanes;
    s << ph.BytesPerLine;
    s << ph.PaletteInfo;
    s << ph.HScreenSize;
    s << ph.VScreenSize;

    quint8 byte = 0;
    for (int i = 0; i < 54; ++i) {
        s << byte;
    }

    return s;
}

PCXHEADER::PCXHEADER()
{
    // Initialize all data to zero
    QByteArray dummy(128, 0);
    dummy.fill(0);
    QDataStream s(&dummy, QIODevice::ReadOnly);
    s >> *this;
}

static void readLine(QDataStream &s, QByteArray &buf, const PCXHEADER &header)
{
    quint32 i = 0;
    quint32 size = buf.size();
    quint8 byte, count;

    if (header.isCompressed()) {
        // Uncompress the image data
        while (i < size) {
            count = 1;
            s >> byte;
            if (byte > 0xc0) {
                count = byte - 0xc0;
                s >> byte;
            }
            while (count-- && i < size) {
                buf[ i++ ] = byte;
            }
        }
    } else {
        // Image is not compressed (possible?)
        while (i < size) {
            s >> byte;
            buf[ i++ ] = byte;
        }
    }
}

static void readImage1(QImage &img, QDataStream &s, const PCXHEADER &header)
{
    QByteArray buf(header.BytesPerLine, 0);

    img = QImage(header.width(), header.height(), QImage::Format_Mono);
    img.setColorCount(2);

    for (int y = 0; y < header.height(); ++y) {
        if (s.atEnd()) {
            img = QImage();
            return;
        }

        readLine(s, buf, header);
        uchar *p = img.scanLine(y);
        unsigned int bpl = qMin((quint16)((header.width() + 7) / 8), header.BytesPerLine);
        for (unsigned int x = 0; x < bpl; ++x) {
            p[ x ] = buf[x];
        }
    }

    // Set the color palette
    img.setColor(0, qRgb(0, 0, 0));
    img.setColor(1, qRgb(255, 255, 255));
}

static void readImage4(QImage &img, QDataStream &s, const PCXHEADER &header)
{
    QByteArray buf(header.BytesPerLine * 4, 0);
    QByteArray pixbuf(header.width(), 0);

    img = QImage(header.width(), header.height(), QImage::Format_Indexed8);
    img.setColorCount(16);

    for (int y = 0; y < header.height(); ++y) {
        if (s.atEnd()) {
            img = QImage();
            return;
        }

        pixbuf.fill(0);
        readLine(s, buf, header);

        for (int i = 0; i < 4; i++) {
            quint32 offset = i * header.BytesPerLine;
            for (int x = 0; x < header.width(); ++x)
                if (buf[ offset + (x / 8) ] & (128 >> (x % 8))) {
                    pixbuf[ x ] = (int)(pixbuf[ x ]) + (1 << i);
                }
        }

        uchar *p = img.scanLine(y);
        for (int x = 0; x < header.width(); ++x) {
            p[ x ] = pixbuf[ x ];
        }
    }

    // Read the palette
    for (int i = 0; i < 16; ++i) {
        img.setColor(i, header.ColorMap.color(i));
    }
}

static void readImage8(QImage &img, QDataStream &s, const PCXHEADER &header)
{
    QByteArray buf(header.BytesPerLine, 0);

    img = QImage(header.width(), header.height(), QImage::Format_Indexed8);
    img.setColorCount(256);

    for (int y = 0; y < header.height(); ++y) {
        if (s.atEnd()) {
            img = QImage();
            return;
        }

        readLine(s, buf, header);

        uchar *p = img.scanLine(y);
        unsigned int bpl = qMin(header.BytesPerLine, (quint16)header.width());
        for (unsigned int x = 0; x < bpl; ++x) {
            p[ x ] = buf[ x ];
        }
    }

    quint8 flag;
    s >> flag;
//   qDebug() << "Palette Flag: " << flag;

    if (flag == 12 && (header.Version == 5 || header.Version == 2)) {
        // Read the palette
        quint8 r, g, b;
        for (int i = 0; i < 256; ++i) {
            s >> r >> g >> b;
            img.setColor(i, qRgb(r, g, b));
        }
    }
}

static void readImage24(QImage &img, QDataStream &s, const PCXHEADER &header)
{
    QByteArray r_buf(header.BytesPerLine, 0);
    QByteArray g_buf(header.BytesPerLine, 0);
    QByteArray b_buf(header.BytesPerLine, 0);

    img = QImage(header.width(), header.height(), QImage::Format_RGB32);

    for (int y = 0; y < header.height(); ++y) {
        if (s.atEnd()) {
            img = QImage();
            return;
        }

        readLine(s, r_buf, header);
        readLine(s, g_buf, header);
        readLine(s, b_buf, header);

        uint *p = (uint *)img.scanLine(y);
        for (int x = 0; x < header.width(); ++x) {
            p[ x ] = qRgb(r_buf[ x ], g_buf[ x ], b_buf[ x ]);
        }
    }
}

static void writeLine(QDataStream &s, QByteArray &buf)
{
    quint32 i = 0;
    quint32 size = buf.size();
    quint8 count, data;
    char byte;

    while (i < size) {
        count = 1;
        byte = buf[ i++ ];

        while ((i < size) && (byte == buf[ i ]) && (count < 63)) {
            ++i;
            ++count;
        }

        data = byte;

        if (count > 1 || data >= 0xc0) {
            count |= 0xc0;
            s << count;
        }

        s << data;
    }
}

static void writeImage1(QImage &img, QDataStream &s, PCXHEADER &header)
{
    img = img.convertToFormat(QImage::Format_Mono);

    header.Bpp = 1;
    header.NPlanes = 1;
    header.BytesPerLine = img.bytesPerLine();

    s << header;

    QByteArray buf(header.BytesPerLine, 0);

    for (int y = 0; y < header.height(); ++y) {
        quint8 *p = img.scanLine(y);

        // Invert as QImage uses reverse palette for monochrome images?
        for (int i = 0; i < header.BytesPerLine; ++i) {
            buf[ i ] = ~p[ i ];
        }

        writeLine(s, buf);
    }
}

static void writeImage4(QImage &img, QDataStream &s, PCXHEADER &header)
{
    header.Bpp = 1;
    header.NPlanes = 4;
    header.BytesPerLine = header.width() / 8;

    for (int i = 0; i < 16; ++i) {
        header.ColorMap.setColor(i, img.color(i));
    }

    s << header;

    QByteArray buf[ 4 ];

    for (int i = 0; i < 4; ++i) {
        buf[ i ].resize(header.BytesPerLine);
    }

    for (int y = 0; y < header.height(); ++y) {
        quint8 *p = img.scanLine(y);

        for (int i = 0; i < 4; ++i) {
            buf[ i ].fill(0);
        }

        for (int x = 0; x < header.width(); ++x) {
            for (int i = 0; i < 4; ++i)
                if (*(p + x) & (1 << i)) {
                    buf[ i ][ x / 8 ] = (int)(buf[ i ][ x / 8 ]) | 1 << (7 - x % 8);
                }
        }

        for (int i = 0; i < 4; ++i) {
            writeLine(s, buf[ i ]);
        }
    }
}

static void writeImage8(QImage &img, QDataStream &s, PCXHEADER &header)
{
    header.Bpp = 8;
    header.NPlanes = 1;
    header.BytesPerLine = img.bytesPerLine();

    s << header;

    QByteArray buf(header.BytesPerLine, 0);

    for (int y = 0; y < header.height(); ++y) {
        quint8 *p = img.scanLine(y);

        for (int i = 0; i < header.BytesPerLine; ++i) {
            buf[ i ] = p[ i ];
        }

        writeLine(s, buf);
    }

    // Write palette flag
    quint8 byte = 12;
    s << byte;

    // Write palette
    for (int i = 0; i < 256; ++i) {
        s << RGB::from(img.color(i));
    }
}

static void writeImage24(QImage &img, QDataStream &s, PCXHEADER &header)
{
    header.Bpp = 8;
    header.NPlanes = 3;
    header.BytesPerLine = header.width();

    s << header;

    QByteArray r_buf(header.width(), 0);
    QByteArray g_buf(header.width(), 0);
    QByteArray b_buf(header.width(), 0);

    for (int y = 0; y < header.height(); ++y) {
        uint *p = (uint *)img.scanLine(y);

        for (int x = 0; x < header.width(); ++x) {
            QRgb rgb = *p++;
            r_buf[ x ] = qRed(rgb);
            g_buf[ x ] = qGreen(rgb);
            b_buf[ x ] = qBlue(rgb);
        }

        writeLine(s, r_buf);
        writeLine(s, g_buf);
        writeLine(s, b_buf);
    }
}

PCXHandler::PCXHandler()
{
}

bool PCXHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("pcx");
        return true;
    }
    return false;
}

bool PCXHandler::read(QImage *outImage)
{
    QDataStream s(device());
    s.setByteOrder(QDataStream::LittleEndian);

    if (s.device()->size() < 128) {
        return false;
    }

    PCXHEADER header;

    s >> header;

    if (header.Manufacturer != 10 || s.atEnd()) {
        return false;
    }

//   int w = header.width();
//   int h = header.height();

//   qDebug() << "Manufacturer: " << header.Manufacturer;
//   qDebug() << "Version: " << header.Version;
//   qDebug() << "Encoding: " << header.Encoding;
//   qDebug() << "Bpp: " << header.Bpp;
//   qDebug() << "Width: " << w;
//   qDebug() << "Height: " << h;
//   qDebug() << "Window: " << header.XMin << "," << header.XMax << ","
//                  << header.YMin << "," << header.YMax << endl;
//   qDebug() << "BytesPerLine: " << header.BytesPerLine;
//   qDebug() << "NPlanes: " << header.NPlanes;

    QImage img;

    if (header.Bpp == 1 && header.NPlanes == 1) {
        readImage1(img, s, header);
    } else if (header.Bpp == 1 && header.NPlanes == 4) {
        readImage4(img, s, header);
    } else if (header.Bpp == 8 && header.NPlanes == 1) {
        readImage8(img, s, header);
    } else if (header.Bpp == 8 && header.NPlanes == 3) {
        readImage24(img, s, header);
    }

//   qDebug() << "Image Bytes: " << img.numBytes();
//   qDebug() << "Image Bytes Per Line: " << img.bytesPerLine();
//   qDebug() << "Image Depth: " << img.depth();

    if (!img.isNull()) {
        *outImage = img;
        return true;
    } else {
        return false;
    }
}

bool PCXHandler::write(const QImage &image)
{
    QDataStream s(device());
    s.setByteOrder(QDataStream::LittleEndian);

    QImage img = image;

    int w = img.width();
    int h = img.height();

//   qDebug() << "Width: " << w;
//   qDebug() << "Height: " << h;
//   qDebug() << "Depth: " << img.depth();
//   qDebug() << "BytesPerLine: " << img.bytesPerLine();
//   qDebug() << "Color Count: " << img.colorCount();

    PCXHEADER header;

    header.Manufacturer = 10;
    header.Version = 5;
    header.Encoding = 1;
    header.XMin = 0;
    header.YMin = 0;
    header.XMax = w - 1;
    header.YMax = h - 1;
    header.HDpi = 300;
    header.YDpi = 300;
    header.Reserved = 0;
    header.PaletteInfo = 1;

    if (img.depth() == 1) {
        writeImage1(img, s, header);
    } else if (img.depth() == 8 && img.colorCount() <= 16) {
        writeImage4(img, s, header);
    } else if (img.depth() == 8) {
        writeImage8(img, s, header);
    } else if (img.depth() == 32) {
        writeImage24(img, s, header);
    }

    return true;
}

bool PCXHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("PCXHandler::canRead() called with no device");
        return false;
    }

    qint64 oldPos = device->pos();

    char head[1];
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

    return qstrncmp(head, "\012", 1) == 0;
}

QImageIOPlugin::Capabilities PCXPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "pcx") {
        return Capabilities(CanRead | CanWrite);
    }
    if (!format.isEmpty()) {
        return nullptr;
    }
    if (!device->isOpen()) {
        return nullptr;
    }

    Capabilities cap;
    if (device->isReadable() && PCXHandler::canRead(device)) {
        cap |= CanRead;
    }
    if (device->isWritable()) {
        cap |= CanWrite;
    }
    return cap;
}

QImageIOHandler *PCXPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new PCXHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
