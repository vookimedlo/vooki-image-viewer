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


void ImageProcessor::bind(const QImage &image)
{
    // shallow copy
    m_originalImage = image;

    resetTransformation();
}

QImage ImageProcessor::process()
{
    if (m_originalImage.isNull())
        return m_originalImage;

    QImage scaledImage;
    QImage rotatedImage = m_originalImage.transformed(QTransform().rotate(m_rotateIndex * 90), Qt::SmoothTransformation);

    // It seems that Qt implementation has swapped the meaning of the vertical and horizontal flip
    // rotatedImage = rotatedImage.mirrored(m_flipHorizontally, m_flipVertically);

    if (m_flipHorizontally)
    {
        QTransform transform;
        QTransform trans = transform.rotate(180, Qt::XAxis);
        rotatedImage = rotatedImage.transformed(trans, Qt::SmoothTransformation);
    }

    if (m_flipVertically)
    {
        QTransform transform;
        QTransform trans = transform.rotate(180, Qt::YAxis);
        rotatedImage = rotatedImage.transformed(trans, Qt::SmoothTransformation);
    }

    if (m_fitToArea)
    {
        //m_imageOffsetX = m_imageOffsetY = 0;
        scaledImage = rotatedImage.scaledToWidth(m_areaWidth, Qt::SmoothTransformation);
        if (scaledImage.height() > m_areaHeight)
            scaledImage = rotatedImage.scaledToHeight(m_areaHeight, Qt::SmoothTransformation);
    }
    else
        scaledImage = rotatedImage.scaledToWidth((int)(rotatedImage.width() * m_scaleFactor), Qt::SmoothTransformation);

    /*
    QSize newSize = scaledImage.size().expandedTo(size());
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Settings::userSettings()->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>());

    // Update scroll settings
    checkScrollOffset();
    QPainter painterImage(&newImage);
    painterImage.drawImage(newSize.width() / 2 - scaledImage.size().width() / 2,
                           newSize.height() / 2 - scaledImage.size().height() / 2,
                           scaledImage,
                           m_imageOffsetX,
                           m_imageOffsetY);

    if (m_drawBorder)
    {
        painterImage.setBrush(Qt::NoBrush);
        QPen pen = painterImage.pen();
        pen.setWidth(3);
        pen.setColor(m_borderColor);
        painterImage.setPen(pen);
        painterImage.drawRect((newSize.width() / 2 - scaledImage.size().width() / 2) - m_imageOffsetX,
                              (newSize.height() / 2 - scaledImage.size().height() / 2) - m_imageOffsetY,
                              scaledImage.width(),
                              scaledImage.height());
    }

    m_finalImage = newImage;

    if (!m_originalImage.isNull())
        emit zoomPercentageChanged(scaledImage.width() / static_cast<qreal>(m_originalImage.width()));
        */

    return scaledImage;
}

void ImageProcessor::setAreaSize(const QSize &size)
{
    m_areaHeight = size.height();
    m_areaWidth = size.width();
}

void ImageProcessor::setScaleFactor(double value)
{
    m_scaleFactor = value;
}

void ImageProcessor::flipHorizontally()
{
    m_flipHorizontally = !m_flipHorizontally;

    if (m_flipHorizontally && m_flipVertically)
    {
        // Simultaneous vertical and horizontal flip is equal to the PI RAD (180 degrees) rotation
        m_flipHorizontally = m_flipVertically = false;
        ++m_rotateIndex;
        ++m_rotateIndex;
    }
}

void ImageProcessor::flipVertically()
{
    m_flipVertically = !m_flipVertically;

    if (m_flipHorizontally && m_flipVertically)
    {
        // Simultaneous vertical and horizontal flip is equal to the PI RAD (180 degrees) rotation
        m_flipHorizontally = m_flipVertically = false;
        ++m_rotateIndex;
        ++m_rotateIndex;
    }
}

void ImageProcessor::rotateLeft()
{
    if (m_flipHorizontally || m_flipVertically)
        ++m_rotateIndex;
    else
        --m_rotateIndex;
}

void ImageProcessor::rotateRight()
{
    if (m_flipHorizontally || m_flipVertically)
        --m_rotateIndex;
    else
        ++m_rotateIndex;
}

void ImageProcessor::resetTransformation()
{
    m_flipHorizontally = m_flipVertically = false;
    //m_imageOffsetX = m_imageOffsetY = 0;
    m_rotateIndex.reset(0);
    m_scaleFactor = 1.0;
}

double ImageProcessor::getScaleFactor() const
{
    return m_scaleFactor;
}

void ImageProcessor::setFitToArea(bool fitToArea)
{
    m_fitToArea = fitToArea;
}

bool ImageProcessor::isFitToAreaEnabled() const
{
    return m_fitToArea;
}
