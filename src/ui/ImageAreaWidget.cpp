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

#include <cmath>
#include <QColor>
#include <QDebug>
#include <QImage>
#include <QNativeGestureEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QWheelEvent>
#include "support/Settings.h"
#include "support/SettingsStrings.h"

const int ImageAreaWidget::m_imageOffsetStep = 100;

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
                                       m_imageOffsetY(0),
                                       m_imageOffsetX(0)
{
    m_originalImage.fill(qRgb(0, 0, 0));
    m_finalImage.fill(qRgb(0, 0, 0));
}

void ImageAreaWidget::onIncreaseOffsetY(int pixels)
{
    m_imageOffsetY += pixels;
    repaintWithTransformations();
}

void ImageAreaWidget::onIncreaseOffsetX(int pixels)
{
    m_imageOffsetX += pixels;
    repaintWithTransformations();
}

void ImageAreaWidget::onDecreaseOffsetY(int pixels)
{
    m_imageOffsetY -= pixels;
    repaintWithTransformations();
}

void ImageAreaWidget::onDecreaseOffsetX(int pixels)
{
    m_imageOffsetX -= pixels;
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

    if(!m_originalImage.isNull())
        emit zoomPercentageChanged(scaledImage.width()/(qreal)m_originalImage.width());
}

void ImageAreaWidget::onFlipHorizontallyTriggered()
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

void ImageAreaWidget::onFlipVerticallyTriggered()
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

void ImageAreaWidget::onSetFitToWindowTriggered(bool enabled)
{
    m_isFitToWindow = enabled;
    m_scaleFactor = 1;
    transformImage();
    update();
}

void ImageAreaWidget::onRotateLeftTriggered()
{
    if(m_flipHorizontally || m_flipVertically)
        ++m_rotateIndex;
    else
        --m_rotateIndex;
    transformImage();
    update();
}

void ImageAreaWidget::onRotateRightTriggered()
{
    if(m_flipHorizontally || m_flipVertically)
        --m_rotateIndex;
    else
        ++m_rotateIndex;
     transformImage();
     update();
}

void ImageAreaWidget::onZoomImageInTriggered(double factor)
{
    const double maxValue = 2.0;
    double newScaleFactor = factor + m_scaleFactor;
    m_scaleFactor = newScaleFactor < maxValue ? newScaleFactor : maxValue;
    transformImage();
    update();
}

void ImageAreaWidget::onZoomImageOutTriggered(double factor)
{
    const double minValue = 0.1;
    double newScaleFactor = -factor + m_scaleFactor;
    m_scaleFactor = newScaleFactor > minValue ? newScaleFactor : minValue;
    transformImage();
    update();
}

void ImageAreaWidget::onZoomResetTriggered()
{
    bool isFitToWindow = this->m_isFitToWindow;
    this->m_isFitToWindow = false;
    m_scaleFactor = 1;
    transformImage();
    update();
    this->m_isFitToWindow = isFitToWindow;
}

void ImageAreaWidget::scroll(const QPoint &point)
{
    if (point.x() >= 0)
        onDecreaseOffsetX(point.x());
    else
        onIncreaseOffsetX(-point.x());

    if (point.y() >= 0)
        onDecreaseOffsetY(point.y());
    else
        onIncreaseOffsetY(-point.y());
}

void ImageAreaWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        scroll(numPixels);
    }
    else if (!numDegrees.isNull())
    {
        QPoint numSteps = numDegrees / 15;
        numSteps.rx() *= m_imageOffsetStep;
        numSteps.ry() *= m_imageOffsetStep;
        scroll(numPixels);
    }

    event->accept();
}

bool ImageAreaWidget::event(QEvent *ev)
{
   switch (ev->type())
   {
   case QEvent::NativeGesture:
       nativeGestureEvent(static_cast<QNativeGestureEvent*>(ev));
       break;
   default:
       return QWidget::event(ev);
   }

   return ev->isAccepted();
}

void ImageAreaWidget::nativeGestureEvent(QNativeGestureEvent *event)
{
    static qreal zoomPercentage = 0;

    switch(event->gestureType())
    {
    case Qt::EndNativeGesture:
        if (zoomPercentage)
        {
            gestureZoom(zoomPercentage);
            zoomPercentage = 0;
        }
        break;
    case Qt::ZoomNativeGesture:
        zoomPercentage += event->value();

        // Redraw image in 0.10 steps
        if (std::abs(zoomPercentage) > 0.10)
        {
            gestureZoom(zoomPercentage);
            zoomPercentage = 0;
        }
        break;
    case Qt::SmartZoomNativeGesture:
    {
        const double factor = 1000;
        if (event->value())
            onZoomImageInTriggered(factor);
        else
            onZoomImageOutTriggered(factor);
        break;
    }
    default:
        ; // nothing here - intentionally
    }

    event->accept();
}

void ImageAreaWidget::gestureZoom(qreal value)
{
    qDebug() << "before " << m_scaleFactor;
    value /= 5;
    if (value > 0)
        onZoomImageInTriggered(value);
    else
        onZoomImageOutTriggered(-value);
    qDebug() << "after " << m_scaleFactor;
}

void ImageAreaWidget::onScrollLeftTriggered()
{
    onIncreaseOffsetX();
}

void ImageAreaWidget::onScrollRightTriggered()
{
    onDecreaseOffsetX();
}

void ImageAreaWidget::onScrollUpTriggered()
{
    onIncreaseOffsetY();
}

void ImageAreaWidget::onScrollDownTriggered()
{
    onDecreaseOffsetY();
}
