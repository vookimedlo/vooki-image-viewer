/****************************************************************************
VookiImageViewer - tool to showing images.
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

#include "ImageArea.h"

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>

ImageArea::ImageArea(QWidget *parent): QWidget(parent),
                                       m_isFitToWindow(false),
                                       m_scaleFactor(1.0),
                                       m_originalImage(),
                                       m_finalImage(),
                                       m_rotateIndex(0, 4) // 4 rotation quadrants
{
    m_originalImage.fill(qRgb(0, 0, 0));
    m_finalImage.fill(qRgb(0, 0, 0));
}

bool ImageArea::showImage(const QString &fileName)
{
    if (!m_originalImage.load(fileName))
        return false;
    m_rotateIndex.reset(0);
    m_scaleFactor = 1;
    transformImage();
    update();
    return true;
}

void ImageArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_finalImage, dirtyRect);
}

void ImageArea::resizeEvent(QResizeEvent *event)
{
    transformImage();
    update();
    QWidget::resizeEvent(event);
}

void ImageArea::transformImage()
{
    QTransform transform;
    QTransform trans = transform.rotate(m_rotateIndex * 90);
    QImage rotatedImage = m_originalImage.transformed(trans);
    QImage scaledImage;

    if(m_isFitToWindow)
    {
        scaledImage = rotatedImage.scaledToWidth(width());
        if(scaledImage.height() > height())
            scaledImage = rotatedImage.scaledToHeight(height());
    }
    else
        scaledImage = rotatedImage.scaledToWidth(rotatedImage.width() * m_scaleFactor);

    QSize newSize = scaledImage.size().expandedTo(size());
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(0, 0, 0));
    QPainter painterImage(&newImage);
    painterImage.drawImage(QPoint(newSize.width() / 2 - scaledImage.size().width() / 2, newSize.height() / 2 - scaledImage.size().height() / 2), scaledImage);
    m_finalImage = newImage;
}

void ImageArea::setFitToWindow(bool enabled)
{
    m_isFitToWindow = enabled;
    m_scaleFactor = 1;
    transformImage();
    update();
}

void ImageArea::rotateLeft()
{
    --m_rotateIndex;
    transformImage();
    update();
}

void ImageArea::rotateRight()
{
     ++m_rotateIndex;
     transformImage();
     update();
}

void ImageArea::zoomImageIn(double factor)
{
    double newScaleFactor = factor + m_scaleFactor;
    if(newScaleFactor > 3.0)
        return;

    m_scaleFactor = newScaleFactor;
    transformImage();
    update();
}

void ImageArea::zoomImageOut(double factor)
{
    double newScaleFactor = -1 * factor + m_scaleFactor;
    if(newScaleFactor < 0.2)
        return;

    m_scaleFactor = newScaleFactor;
    transformImage();
    update();
}

void ImageArea::zoomReset()
{
    bool isFitToWindow = this->m_isFitToWindow;
    this->m_isFitToWindow = false;
    m_scaleFactor = 1;
    transformImage();
    update();
    this->m_isFitToWindow = isFitToWindow;
}
