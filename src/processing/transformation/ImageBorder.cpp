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

#include "ImageBorder.h"
#include <QPainter>


void ImageBorder::checkScrollOffset(const QImage &image)
{
    if (image.height() < m_areaSize.height())
        m_imageOffsetY = 0;
    else if (image.height() - m_imageOffsetY < m_areaSize.height())
        m_imageOffsetY = image.height() - m_areaSize.height();

    if (image.width() < m_areaSize.width())
        m_imageOffsetX = 0;
    else if (image.width() - m_imageOffsetX < m_areaSize.width())
        m_imageOffsetX = image.width() - m_areaSize.width();

    if (m_imageOffsetY < 0)
        m_imageOffsetY = 0;

    if (m_imageOffsetX < 0)
        m_imageOffsetX = 0;
}

void ImageBorder::setAreaSize(const QSize &size)
{
    m_areaSize = size;
    setIsCacheDirty(true);
}

QImage ImageBorder::transform()
{
    if (isCacheDirty())
    {
        QImage originalImage = getOriginalImage();
        QSize newSize = originalImage.size().expandedTo(m_areaSize);
        QImage newImage(newSize, QImage::Format_RGB32);
        // newImage.fill(Settings::userSettings()->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>());

        // Update scroll settings
        checkScrollOffset(newImage);
        QPainter painterImage(&newImage);
        painterImage.drawImage(newSize.width() / 2 - originalImage.size().width() / 2,
                               newSize.height() / 2 - originalImage.size().height() / 2,
                               originalImage,
                               m_imageOffsetX,
                               m_imageOffsetY);
        /*
            if (m_drawBorder)
            {
                painterImage.setBrush(Qt::NoBrush);
                QPen pen = painterImage.pen();
                pen.setWidth(3);
                pen.setColor(m_borderColor);
                painterImage.setPen(pen);
                painterImage.drawRect((newSize.width() / 2 - scaledImage.width() / 2) - m_imageOffsetX,
                                      (newSize.height() / 2 - scaledImage.height() / 2) - m_imageOffsetY,
                                      scaledImage.width(),
                                      scaledImage.height());
            }
            */

        setCachedImage(newImage);
    }

    return getCachedImage();
}

void ImageBorder::resetProperties()
{
    ImageTransformation::resetProperties();
}

