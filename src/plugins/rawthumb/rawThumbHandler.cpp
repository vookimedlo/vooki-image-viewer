/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
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
    auto state = raw.open_buffer(buffer.data(), buffer.size());
    if (LIBRAW_SUCCESS == state)
    {
        QImage thumbnail;
        if (LIBRAW_SUCCESS == raw.unpack_thumb())
        {
            if (LIBRAW_THUMBNAIL_JPEG == raw.imgdata.thumbnail.tformat)
            {
                thumbnail.loadFromData((unsigned char *)raw.imgdata.thumbnail.thumb, raw.imgdata.thumbnail.tlength, "JPEG");
            }
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
    auto state = raw.open_buffer(buffer.data(), buffer.size());
    if (LIBRAW_SUCCESS == state)
    {
        QImage thumbnail;
        if (LIBRAW_SUCCESS == raw.unpack_thumb())
        {
            raw.recycle();
            return true;
        }
    }

    raw.recycle();
    return false;
}
