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

#include "rawThumbPlugin.h"
#include "rawThumbHandler.h"
#include <QStringList>

QImageIOPlugin::Capabilities RawThumbPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    static const QStringList formats = { "arw", "crw", "cr2", "dng", "nef", "raf" };
    if (formats.contains(format))
    {
        return Capabilities(CanRead);
    }

    if (!format.isEmpty())
    {
        return nullptr;
    }

    if (!device || !device->isOpen())
    {
        return nullptr;
    }

    Capabilities capabilities;
    if (device->isReadable() && RawThumbHandler::canRead(device))
    {
        capabilities |= CanRead;
    }

    return capabilities;
}

QImageIOHandler *RawThumbPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new RawThumbHandler();
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
