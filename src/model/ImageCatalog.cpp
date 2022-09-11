/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include "ImageCatalog.h"

#include <utility>

ImageCatalog::ImageCatalog(QStringList filter)
                                        : m_catalogIndex()
                                        , m_filter(std::move(filter))
{
}

void ImageCatalog::initialize(const QFile &imageFile)
{
    QFileInfo info(imageFile);
    const QString filename = info.fileName();
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

int ImageCatalog::getCatalogSize() const
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

QString ImageCatalog::getCatalogItem(const RotatingIndex<int> &catalogIndex) const
{
    if (m_catalog.isEmpty())
        return QString();

    return QFileInfo(m_absoluteDir + QDir::separator() + m_catalog.at(catalogIndex)).canonicalFilePath();
}
