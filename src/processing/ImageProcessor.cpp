/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageProcessor.h"
#include <algorithm>

void ImageProcessor::bind(const QImage &image, bool resetTransformation)
{
    // shallow copy
    m_originalImage = image;
    resetTransformation ? ImageProcessor::resetTransformation() : m_genericTransformations.front()->setIsCacheDirty(true);
}

QImage ImageProcessor::process()
{
    if (m_originalImage.isNull())
        return m_originalImage;

    m_imageZoom.setOriginalImageSize(m_originalImage.size());

    QTransform lastTransformation {};
    bool needsTransformation { m_transformations.front()->isCacheDirty() };
    for (auto const transformation : m_transformations)
    {
        // Mark as dirty if the previous transformation in stack was dirty too.
        if (needsTransformation)
            transformation->bind(lastTransformation);
        else if (transformation->isCacheDirty())
            needsTransformation = true;

        lastTransformation = transformation->transform().value<QTransform>();
    }

    needsTransformation = needsTransformation || m_imageTransformations.front()->isCacheDirty();
    if (needsTransformation)
    {
        QImage lastTransformedImage = m_originalImage.transformed(lastTransformation, Qt::SmoothTransformation);
        for (auto const transformation : m_imageTransformations)
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
    else
    {
        return m_transformations.front()->transform().value<QImage>();
    }
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

void ImageProcessor::resetTransformation() const
{
    std::for_each(
      m_genericTransformations.cbegin(), m_genericTransformations.cend(),
                  [](auto const transformation){transformation->resetProperties();});
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
