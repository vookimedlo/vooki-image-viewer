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

#include "ImageFlip.h"


void ImageFlip::flipHorizontally()
{
    m_flipHorizontally = !m_flipHorizontally;
    setIsCacheDirty(true);
}

void ImageFlip::flipVertically()
{
    m_flipVertically = !m_flipVertically;
    setIsCacheDirty(true);
}

bool ImageFlip::isFlippedHorizontally() const
{
    return m_flipHorizontally;
}

bool ImageFlip::isFlippedVertically() const
{
    return m_flipVertically;
}

void ImageFlip::setFlipHorizontally(bool flipHorizontally)
{
    m_flipHorizontally = flipHorizontally;
}

void ImageFlip::setFlipVertically(bool flipVertically)
{
    m_flipVertically = flipVertically;
}


QImage ImageFlip::transform()
{
    if (isCacheDirty())
    {
        QImage newImage = getOriginalImage();
        if (m_flipHorizontally)
        {
            static QTransform transform{QTransform().rotate(180, Qt::XAxis)};
            newImage = newImage.transformed(transform, Qt::SmoothTransformation);
        }

        if (m_flipVertically)
        {
            static QTransform transform{QTransform().rotate(180, Qt::YAxis)};
            newImage = newImage.transformed(transform, Qt::SmoothTransformation);
        }

        setCachedImage(newImage);
    }
    return getCachedImage();
}

void ImageFlip::resetProperties()
{
    m_flipHorizontally = m_flipVertically = false;
    ImageTransformation::resetProperties();
}
