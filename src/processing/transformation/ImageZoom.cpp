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

#include "ImageZoom.h"


void ImageZoom::setAreaSize(const QSize &size)
{
    m_areaHeight = size.height();
    m_areaWidth = size.width();
}

double ImageZoom::getScaleFactor() const
{
    return m_scaleFactor;
}

void ImageZoom::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
    invalidateCache();
}

bool ImageZoom::isFitToAreaEnabled() const
{
    return m_fitToArea;
}

void ImageZoom::setFitToArea(bool fitToArea)
{
    m_fitToArea = fitToArea;
    invalidateCache();
}

QImage ImageZoom::transform()
{
    if (isCacheDirty()) {
        QImage originalImage = getOriginalImage();
        setCachedImage([this, originalImage]() {
            if (m_fitToArea)
            {
                if ((static_cast<double>(m_areaWidth) / originalImage.width() * originalImage.height()) <= m_areaHeight)
                    return originalImage.scaledToWidth(m_areaWidth, Qt::SmoothTransformation);
                else
                    return originalImage.scaledToHeight(m_areaHeight, Qt::SmoothTransformation);
            }
            else
                return originalImage.scaledToWidth((int)(originalImage.width() * m_scaleFactor), Qt::SmoothTransformation);
        }());

        m_scaleFactor = getCachedImage().width() / static_cast<double>(originalImage.width());
    }
    return getCachedImage();
}

void ImageZoom::resetProperties()
{
    m_scaleFactor = 1.0;
    ImageTransformation::resetProperties();
}
