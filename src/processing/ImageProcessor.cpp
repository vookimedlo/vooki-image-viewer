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

#include "ImageProcessor.h"
#include <algorithm>

void ImageProcessor::bind(const QImage &image, bool resetTransformation)
{
    // shallow copy
    m_originalImage = image;
    (resetTransformation) ? ImageProcessor::resetTransformation() : m_transformations[0]->setIsCacheDirty(true);
}

QImage ImageProcessor::process()
{
    if (m_originalImage.isNull())
        return m_originalImage;

    QImage lastTransformedImage = m_originalImage;
    bool needsTransformation { m_transformations[0]->isCacheDirty() };
    for (auto const transformation : m_transformations)
    {
        // Mark as dirty if the previous transformation in stack was dirty too.
        if (needsTransformation)
            transformation->bind(lastTransformedImage);
        else if (transformation->isCacheDirty())
            needsTransformation = true;

        lastTransformedImage = transformation->transform().value<QImage>();
    }

    return lastTransformedImage;
}

void ImageProcessor::setAreaSize(const QSize &size)
{
    m_imageZoom.setAreaSize(size);
    m_imageBorder.setAreaSize(size);
}

void ImageProcessor::setScaleFactor(double value)
{
    m_imageZoom.setScaleFactor(value);
}

void ImageProcessor::flip()
{
    if (m_imageFlip.isFlippedHorizontally() && m_imageFlip.isFlippedVertically())
    {
        // Simultaneous vertical and horizontal flip is equal to the PI RAD (180 degrees) rotation
        m_imageFlip.flipHorizontally();
        m_imageFlip.flipVertically();
        m_imageRotation.rotateRight();
        m_imageRotation.rotateRight();
    }
}

void ImageProcessor::flipHorizontally()
{
    m_imageFlip.flipHorizontally();
    flip();
}

void ImageProcessor::flipVertically()
{
    m_imageFlip.flipVertically();
    flip();
}

void ImageProcessor::rotateLeft()
{
    if (m_imageFlip.isFlippedHorizontally() || m_imageFlip.isFlippedVertically())
        m_imageRotation.rotateRight();
    else
        m_imageRotation.rotateLeft();
}

void ImageProcessor::rotateRight()
{
    if (m_imageFlip.isFlippedHorizontally() || m_imageFlip.isFlippedVertically())
        m_imageRotation.rotateLeft();
    else
        m_imageRotation.rotateRight();
}

void ImageProcessor::resetTransformation()
{
    std::for_each(m_transformations.cbegin(),
                  m_transformations.cend(),
                  [](auto const transformation){ transformation->resetProperties();});
}

double ImageProcessor::getScaleFactor() const
{
    return m_imageZoom.getScaleFactor();
}

void ImageProcessor::setFitToArea(bool fitToArea)
{
    m_imageZoom.setFitToArea(fitToArea);
}

bool ImageProcessor::isFitToAreaEnabled() const
{
    return m_imageZoom.isFitToAreaEnabled();
}

void ImageProcessor::addImageOffsetY(int imageOffsetY)
{
    m_imageBorder.addImageOffsetY(imageOffsetY);
}

int ImageProcessor::getImageOffsetY() const
{
    return m_imageBorder.getImageOffsetY();
}

void ImageProcessor::setImageOffsetY(int imageOffsetY)
{
    m_imageBorder.setImageOffsetY(imageOffsetY);
}

void ImageProcessor::addImageOffsetX(int imageOffsetX)
{
    m_imageBorder.addImageOffsetX(imageOffsetX);
}

int ImageProcessor::getImageOffsetX() const
{
    return m_imageBorder.getImageOffsetX();
}

void ImageProcessor::setImageOffsetX(int imageOffsetX)
{
    m_imageBorder.setImageOffsetX(imageOffsetX);
}

void ImageProcessor::setBorderColor(const QColor &color)
{
    m_imageBorder.setBorderColor(color);
}

void ImageProcessor::setBackgroundColor(const QColor &color)
{
    m_imageBorder.setBackgroundColor(color);
}

void ImageProcessor::setDrawBorder(bool drawBorder)
{
    m_imageBorder.setDrawBorder(drawBorder);
}
