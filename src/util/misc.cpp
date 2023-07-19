/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "misc.h"
#include <QStringBuilder>

namespace Util
{
    QStringList convertFormatsToFilters(const QList<QByteArray> &formats)
    {
        QStringList filters;
        // Converts formats (e.g. QImageReader::supportedImageFormats()) to QDir::setNameFilters()
        for (const QByteArray &format : formats)
            filters << "*." % QString::fromLatin1(format.toLower());
        return filters;
    }
}
