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

#include "ImageAreaWidget.h"

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include "support/Settings.h"
#include "support/SettingsStrings.h"

ImageAreaWidget::ImageAreaWidget(QWidget *parent): QWidget(parent),
                                       m_drawBorder(false),
                                       m_flipHorizontally(false),
                                       m_flipVertically(false),
                                       m_isFitToWindow(false),
                                       m_scaleFactor(1.0),
                                       m_borderColor(Qt::white),
                                       m_originalImage(),
                                       m_finalImage(),
                                       m_rotateIndex(0, 4), // 4 rotation quadrants
                                       m_imageOffsetStep(200),
                                       m_imageOffsetY(0),
                                       m_imageOffsetX(0)
{
    m_originalImage.fill(qRgb(0, 0, 0));
    m_finalImage.fill(qRgb(0, 0, 0));
}

void ImageAreaWidget::onIncreaseOffsetY()
{
    m_imageOffsetY += m_imageOffsetStep;
    repaintWithTransformations();
}

void ImageAreaWidget::onIncreaseOffsetX()
{
    m_imageOffsetX += m_imageOffsetStep;
    repaintWithTransformations();
}

void ImageAreaWidget::onDecreaseOffsetY()
{
    m_imageOffsetY -= m_imageOffsetStep;
    repaintWithTransformations();
}

void ImageAreaWidget::onDecreaseOffsetX()
{
    m_imageOffsetX -= m_imageOffsetStep;
    repaintWithTransformations();
}

void ImageAreaWidget::checkScrollOffset()
{
    if (m_finalImage.height() < size().height())
        m_imageOffsetY = 0;
    else
    if (m_finalImage.height() - m_imageOffsetY < size().height())
        m_imageOffsetY = m_finalImage.height() - size().height();

    if (m_finalImage.width() < size().width())
        m_imageOffsetX = 0;
    else
    if (m_finalImage.width() - m_imageOffsetX < size().width())
        m_imageOffsetX = m_finalImage.width() - size().width();

    if(m_imageOffsetY < 0)
        m_imageOffsetY = 0;

    if(m_imageOffsetX < 0)
        m_imageOffsetX = 0;
}

void ImageAreaWidget::drawBorder(bool draw, const QColor &color)
{
    m_drawBorder = draw;
    m_borderColor = color;
}

bool ImageAreaWidget::showImage(const QString &fileName)
{
    if (!m_originalImage.load(fileName))
        return false;
    m_flipHorizontally = m_flipVertically = false;
    m_imageOffsetX = m_imageOffsetY = 0;
    m_rotateIndex.reset(0);
    m_scaleFactor = 1;
    transformImage();
    update();
    return true;
}

void ImageAreaWidget::repaintWithTransformations()
{
    transformImage();
    update();
    QWidget::repaint();
}

void ImageAreaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_finalImage, dirtyRect);
}

void ImageAreaWidget::resizeEvent(QResizeEvent *event)
{
    transformImage();
    update();
    QWidget::resizeEvent(event);
}

void ImageAreaWidget::transformImage()
{
    QTransform transform;
    QTransform trans = transform.rotate(m_rotateIndex * 90);
    QImage rotatedImage = m_originalImage.transformed(trans);
    QImage scaledImage;

    // It seems that Qt implementation has swapped the meaning of the vertical and horizontal flip
    //rotatedImage = rotatedImage.mirrored(m_flipHorizontally, m_flipVertically);

    if (m_flipHorizontally)
    {
        QTransform transform;
        QTransform trans = transform.rotate(180, Qt::XAxis);
        rotatedImage = rotatedImage.transformed(trans);
    }

    if (m_flipVertically)
    {
        QTransform transform;
        QTransform trans = transform.rotate(180, Qt::YAxis);
        rotatedImage = rotatedImage.transformed(trans);
    }

    if(m_isFitToWindow)
    {
        m_imageOffsetX = m_imageOffsetY = 0;
        scaledImage = rotatedImage.scaledToWidth(width());
        if(scaledImage.height() > height())
            scaledImage = rotatedImage.scaledToHeight(height());
    }
    else
        scaledImage = rotatedImage.scaledToWidth(rotatedImage.width() * m_scaleFactor);

    QSize newSize = scaledImage.size().expandedTo(size());
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Settings::userSettings()->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>());

    // Update scroll settings
    checkScrollOffset();
    QPainter painterImage(&newImage);
    painterImage.drawImage(newSize.width() / 2 - scaledImage.size().width() / 2, newSize.height() / 2 - scaledImage.size().height() / 2, scaledImage, m_imageOffsetX, m_imageOffsetY);

    if (m_drawBorder)
    {
        painterImage.setBrush(Qt::NoBrush);
        QPen pen = painterImage.pen();
        pen.setWidth(3);
        pen.setColor(m_borderColor);
        painterImage.setPen(pen);
        painterImage.drawRect((newSize.width() / 2 - scaledImage.size().width() / 2) - m_imageOffsetX, (newSize.height() / 2 - scaledImage.size().height() / 2) - m_imageOffsetY, scaledImage.width(), scaledImage.height());
    }

    m_finalImage = newImage;
}

void ImageAreaWidget::onFlipHorizontally()
{
    m_flipHorizontally = !m_flipHorizontally;

    if (m_flipHorizontally && m_flipVertically)
    {
        // Simultanous vertical and horizontal flip is equal to the PI RAD (180degress) rotation
        m_flipHorizontally = m_flipVertically = false;
        ++m_rotateIndex;
        ++m_rotateIndex;
    }

    transformImage();
    update();
}

void ImageAreaWidget::onFlipVertically()
{
    m_flipVertically = !m_flipVertically;

    if (m_flipHorizontally && m_flipVertically)
    {
        // Simultanous vertical and horizontal flip is equal to the PI RAD (180degress) rotation
        m_flipHorizontally = m_flipVertically = false;
        ++m_rotateIndex;
        ++m_rotateIndex;
    }

    transformImage();
    update();
}

void ImageAreaWidget::onSetFitToWindow(bool enabled)
{
    m_isFitToWindow = enabled;
    m_scaleFactor = 1;
    transformImage();
    update();
}

void ImageAreaWidget::onRotateLeft()
{
    if(m_flipHorizontally || m_flipVertically)
        ++m_rotateIndex;
    else
        --m_rotateIndex;
    transformImage();
    update();
}

void ImageAreaWidget::onRotateRight()
{
    if(m_flipHorizontally || m_flipVertically)
        --m_rotateIndex;
    else
        ++m_rotateIndex;
     transformImage();
     update();
}

void ImageAreaWidget::onZoomImageIn(double factor)
{
    double newScaleFactor = factor + m_scaleFactor;
    if(newScaleFactor > 3.0)
        return;

    m_scaleFactor = newScaleFactor;
    transformImage();
    update();
}

void ImageAreaWidget::onZoomImageOut(double factor)
{
    double newScaleFactor = -1 * factor + m_scaleFactor;
    if(newScaleFactor < 0.2)
        return;

    m_scaleFactor = newScaleFactor;
    transformImage();
    update();
}

void ImageAreaWidget::onZoomReset()
{
    bool isFitToWindow = this->m_isFitToWindow;
    this->m_isFitToWindow = false;
    m_scaleFactor = 1;
    transformImage();
    update();
    this->m_isFitToWindow = isFitToWindow;
}
