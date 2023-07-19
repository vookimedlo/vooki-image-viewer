/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
