/* This file is part of the KDE project
   Copyright (C) 2005 Christoph Hormann <chris_hormann@gmx.de>
   Copyright (C) 2005 Ignacio Castaño <castanyo@yahoo.es>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#include "hdr_p.h"

#include <QImage>
#include <QtCore/QDataStream>

#include <QDebug>

typedef Q_UINT8 uchar;

namespace   // Private.
{

#define MAXLINE     1024
#define MINELEN     8       // minimum scanline length for encoding
#define MAXELEN     0x7fff  // maximum scanline length for encoding

static inline uchar ClipToByte(float value)
{
    if (value > 255.0f) {
        return 255;
    }
    //else if (value < 0.0f) return 0;  // we know value is positive.
    return uchar(value);
}

// read an old style line from the hdr image file
// if 'first' is true the first byte is already read
static bool Read_Old_Line(uchar *image, int width, QDataStream &s)
{
    int  rshift = 0;
    int  i;

    while (width > 0) {
        s >> image[0];
        s >> image[1];
        s >> image[2];
        s >> image[3];

        if (s.atEnd()) {
            return false;
        }

        if ((image[0] == 1) && (image[1] == 1) && (image[2] == 1)) {
            for (i = image[3] << rshift; i > 0; i--) {
                //memcpy(image, image-4, 4);
                (uint &)image[0] = (uint &)image[0 - 4];
                image += 4;
                width--;
            }
            rshift += 8;
        } else {
            image += 4;
            width--;
            rshift = 0;
        }
    }
    return true;
}

static void RGBE_To_QRgbLine(uchar *image, QRgb *scanline, int width)
{
    for (int j = 0; j < width; j++) {
        // v = ldexp(1.0, int(image[3]) - 128);
        float v;
        int e = int(image[3]) - 128;
        if (e > 0) {
            v = float(1 << e);
        } else {
            v = 1.0f / float(1 << -e);
        }

        scanline[j] = qRgb(ClipToByte(float(image[0]) * v),
                           ClipToByte(float(image[1]) * v),
                           ClipToByte(float(image[2]) * v));

        image += 4;
    }
}

// Load the HDR image.
static bool LoadHDR(QDataStream &s, const int width, const int height, QImage &img)
{
    uchar val, code;

    // Create dst image.
    if (!img.create(width, height, 32)) {
        return false;
    }

    QMemArray<uchar> image(width * 4);

    for (int cline = 0; cline < height; cline++) {
        QRgb *scanline = (QRgb *) img.scanLine(cline);

        // determine scanline type
        if ((width < MINELEN) || (MAXELEN < width)) {
            Read_Old_Line(image.data(), width, s);
            RGBE_To_QRgbLine(image.data(), scanline, width);
            continue;
        }

        s >> val;

        if (s.atEnd()) {
            return true;
        }

        if (val != 2) {
            s.device()->at(s.device()->at() - 1);
            Read_Old_Line(image.data(), width, s);
            RGBE_To_QRgbLine(image.data(), scanline, width);
            continue;
        }

        s >> image[1];
        s >> image[2];
        s >> image[3];

        if (s.atEnd()) {
            return true;
        }

        if ((image[1] != 2) || (image[2] & 128)) {
            image[0] = 2;
            Read_Old_Line(image.data() + 4, width - 1, s);
            RGBE_To_QRgbLine(image.data(), scanline, width);
            continue;
        }

        if ((image[2] << 8 | image[3]) != width) {
            return false;
        }

        // read each component
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < width;) {
                s >> code;
                if (s.atEnd()) {
                    return false;
                }
                if (code > 128) {
                    // run
                    code &= 127;
                    s >> val;
                    while (code != 0) {
                        image[i + j * 4] = val;
                        j++;
                        code--;
                    }
                } else {
                    // non-run
                    while (code != 0) {
                        s >> image[i +  j * 4];
                        j++;
                        code--;
                    }
                }
            }
        }

        RGBE_To_QRgbLine(image.data(), scanline, width);
    }

    return true;
}

} // namespace

Q_DECL_EXPORT void kimgio_hdr_read(QImageIO *io)
{
    int len;
    char line[MAXLINE];
    //bool validHeader = false;
    bool validFormat = false;

    // Parse header
    do {
        len = io->ioDevice()->readLine(line, MAXLINE);

        /*if (strcmp(line, "#?RADIANCE\n") == 0 || strcmp(line, "#?RGBE\n") == 0)
        {
            validHeader = true;
        }*/
        if (strcmp(line, "FORMAT=32-bit_rle_rgbe\n") == 0) {
            validFormat = true;
        }

    } while ((len > 0) && (line[0] != '\n'));

    if (/*!validHeader ||*/ !validFormat) {
        // qDebug() << "Unknown HDR format.";
        io->setImage(0);
        io->setStatus(-1);
        return;
    }

    io->ioDevice()->readLine(line, MAXLINE);

    char s1[3], s2[3];
    int width, height;
    if (sscanf(line, "%2[+-XY] %d %2[+-XY] %d\n", s1, &height, s2, &width) != 4)
        //if( sscanf(line, "-Y %d +X %d", &height, &width) < 2 )
    {
        // qDebug() << "Invalid HDR file.";
        io->setImage(0);
        io->setStatus(-1);
        return;
    }

    QDataStream s(io->ioDevice());

    QImage img;
    if (!LoadHDR(s, width, height, img)) {
        // qDebug() << "Error loading HDR file.";
        io->setImage(0);
        io->setStatus(-1);
        return;
    }

    io->setImage(img);
    io->setStatus(0);
}

Q_DECL_EXPORT void kimgio_hdr_write(QImageIO *)
{
    // intentionally not implemented (since writing low dynamic range data to a HDR file is nonsense.)
}

