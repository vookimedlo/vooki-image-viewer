/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "rawThumbPlugin.h"
#include <memory>
#include "rawThumbHandler.h"

QImageIOPlugin::Capabilities RawThumbPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (static const QStringList formats = { "raf", "mos", "cr2",
                                             "erf", "dng", "mrw",
                                             "nef", "orf", "rw2",
                                             "pef", "x3f", "srw",
                                             "x3f", "arw" }; formats.contains(format))
    {
        return { CanRead };
    }

    if (!format.isEmpty())
    {
        return {};
    }

    if (!device || !device->isOpen())
    {
        return {};
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
    auto handler = std::make_unique<RawThumbHandler>();
    handler->setDevice(device);
    handler->setFormat(format);
    return handler.release();
}
