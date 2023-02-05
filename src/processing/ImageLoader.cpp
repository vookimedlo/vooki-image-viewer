/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2023  Michal Duda <github@vookimedlo.cz>

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

#include "ImageLoader.h"
#include <QDebug>

bool ImageLoader::loadImage(const QString &fileName)
{
    QImageReader::setAllocationLimit(ImageLoader::m_maxAllocationImageSize);
    m_reader.setFileName(fileName);
    m_reader.setQuality(100);
    m_reader.setAutoTransform(true);

    m_originalImage = QImage();

    if (m_reader.canRead())
    {
        if (isAnimated())
            m_animationIndex.set(0, imageCount());
        return true;
    }

    return false;
}

const QImage &ImageLoader::getImage()
{
    if (m_originalImage.isNull())
        m_reader.read(&m_originalImage);

    return m_originalImage;
}

const QImage &ImageLoader::getNextImage()
{
    if (++m_animationIndex == 0)
        m_reader.setFileName(m_reader.fileName());

    qDebug() << "Index: " << m_animationIndex;

    m_reader.jumpToImage(m_animationIndex);
    m_reader.read(&m_originalImage);

    return m_originalImage;
}

bool ImageLoader::isAnimated() const
{
    return imageCount() != 0;
}

int ImageLoader::imageCount() const
{
    return m_reader.imageCount();
}

int ImageLoader::nextImageDelay() const
{
    return m_reader.nextImageDelay();
}
