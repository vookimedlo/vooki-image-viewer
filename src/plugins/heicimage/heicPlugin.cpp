/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2019  Michal Duda <github@vookimedlo.cz>

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

#include "heicHandler.h"
#include "heicPlugin.h"

QImageIOPlugin::Capabilities HeicPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    static const QStringList formats = { "heic" };

    if (formats.contains(format))
    {
        return { CanRead };
    }

    if (!format.isEmpty())
    {
        return Capabilities();
    }

    if (!device || !device->isOpen())
    {
        return Capabilities();
    }

    Capabilities capabilities;
    if (device->isReadable() && HeicHandler::canRead(device))
    {
        capabilities |= CanRead;
    }

    return capabilities;
}

QImageIOHandler *HeicPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new HeicHandler();
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
