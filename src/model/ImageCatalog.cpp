/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageCatalog.h"

#include <utility>

ImageCatalog::ImageCatalog(QStringList filter)
                                        : m_filter(std::move(filter))
{
}

void ImageCatalog::initialize(const QFile &imageFile)
{
    const QFileInfo info(imageFile);
    const QString &filename {info.fileName()};
    m_absoluteDir = info.absoluteDir().canonicalPath();
    initialize(info.absoluteDir());

    for (const QString &item : m_catalog)
    {
        if (item.compare(filename) == 0)
            break;
        ++m_catalogIndex;
    }
}

void ImageCatalog::initialize(const QDir &imageDir)
{
    m_absoluteDir = imageDir.canonicalPath();
    m_catalog = imageDir.entryList(m_filter, QDir::Filter::Files);
    m_catalog.sort();
    m_catalogIndex.set(0, m_catalog.size());
}

qsizetype ImageCatalog::getCatalogSize() const
{
    return m_catalog.size();
}

QString ImageCatalog::getCurrent() const
{
    return getCatalogItem(m_catalogIndex);
}

QString ImageCatalog::getNext()
{
    return getCatalogItem(++m_catalogIndex);
}

QString ImageCatalog::getPrevious()
{
    return getCatalogItem(--m_catalogIndex);
}

QString ImageCatalog::getCatalogItem(const RotatingIndex<QIntegerForSizeof<std::size_t>::Unsigned> &catalogIndex) const
{
    if (m_catalog.isEmpty())
        return {};

    return QFileInfo{m_absoluteDir + QDir::separator() + m_catalog.at(catalogIndex)}.canonicalFilePath();
}
