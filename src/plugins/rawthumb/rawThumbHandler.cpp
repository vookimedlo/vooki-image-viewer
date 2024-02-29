/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "rawThumbHandler.h"
#include <QImage>
#include <libraw/libraw.h>

bool RawThumbHandler::canRead() const
{
    return canRead(device());
}

bool RawThumbHandler::read(QImage *image)
{
    QByteArray buffer = device()->readAll();
    LibRaw raw;

    if (const auto state = raw.open_buffer(buffer.data(), buffer.size()); LIBRAW_SUCCESS == state)
    {
        QImage thumbnail;
        if (LIBRAW_SUCCESS == raw.unpack_thumb() && LIBRAW_THUMBNAIL_JPEG == raw.imgdata.thumbnail.tformat)
        {
            thumbnail.loadFromData(reinterpret_cast<unsigned char *>(raw.imgdata.thumbnail.thumb), raw.imgdata.thumbnail.tlength, "JPEG");
        }
        raw.recycle();
        *image = thumbnail;
        return true;
    }

    return false;
}

bool RawThumbHandler::canRead(QIODevice *device)
{
    QByteArray buffer = device->readAll();
    device->reset();

    LibRaw raw;
    if (const auto state = raw.open_buffer(buffer.data(), buffer.size()); LIBRAW_SUCCESS == state &&
                                                                             LIBRAW_SUCCESS == raw.unpack_thumb() &&
                                                                             LIBRAW_THUMBNAIL_JPEG == raw.imgdata.thumbnail.tformat)
    {
        raw.recycle();
        return true;
    }

    raw.recycle();
    return false;
}
