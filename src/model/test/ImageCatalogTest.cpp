/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../ImageCatalog.h"
#include "ImageCatalogTest.h"

QString ImageCatalogTest::makeAbsolutePath(const QString &file) const
{
    return QDir::cleanPath(m_absolutePath + QDir::separator() + file);
}

void ImageCatalogTest::noInitialization() const
{
    ImageCatalog imageCatalog {QStringList {}};
    QCOMPARE(imageCatalog.getCurrent(), QString{});
    QCOMPARE(imageCatalog.getNext(), QString{});
    QCOMPARE(imageCatalog.getPrevious(), QString{});
    QCOMPARE(imageCatalog.getCatalogSize(), 0);
}
