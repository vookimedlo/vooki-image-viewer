/****************************************************************************
VookiImageViewer - a tool for showing images.
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
#include <QtConcurrent>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QPaintEvent>
#include <QPainter>
#include <cmath>
#include <qcorofuture.h>
#include "MainWindow.h"
#include "../processing/MetadataExtractor.h"


ImageAreaWidget::ImageAreaWidget(QWidget *parent)
                                        : QWidget(parent)
{
    m_originalImage.fill(qRgb(0, 0, 0));
    m_finalImage.fill(qRgb(0, 0, 0));
}

ImageAreaWidget::~ImageAreaWidget() noexcept
{
}

void ImageAreaWidget::setBackgroundColor(const QColor &color)
{
    m_imageProcessor.setBackgroundColor(color);
}

void ImageAreaWidget::drawBorder(const bool draw, const QColor &color)
{
    m_imageProcessor.setBorderColor(color);
    m_imageProcessor.setDrawBorder(draw);
}

QCoro::Task<bool> ImageAreaWidget::showImage(const QString &fileName)
{
    if (!m_imageLoader.loadImage(fileName))
        co_return false;

    m_originalImage = m_imageLoader.getImage();
    if (m_originalImage.isNull())
        co_return false;

    auto metadataTask = QtConcurrent::run([fileName, this]() {
        extractMetadata(fileName);
    });

    m_imageProcessor.bind(m_originalImage);
    update();

    emit imageDimensionsChanged(m_originalImage.width(), m_originalImage.height());

    transformImage();
    update();

    co_await metadataTask;

    if (m_imageLoader.imageCount() > 1)
    {
        if (const auto delay = m_imageLoader.nextImageDelay(); delay > 0)
            QTimer::singleShot(delay, this, SLOT(onNextImage()));
    }

    co_return true;
}

void ImageAreaWidget::repaintWithTransformations()
{
    transformImage();
    update();
    QWidget::repaint();
}

void ImageAreaWidget::onDecreaseOffsetX(const int pixels)
{
    m_imageProcessor.addImageOffsetX(-pixels);
    repaintWithTransformations();
}

void ImageAreaWidget::onDecreaseOffsetY(const int pixels)
{
    m_imageProcessor.addImageOffsetY(-pixels);
    repaintWithTransformations();
}

void ImageAreaWidget::onFlipHorizontallyTriggered()
{
    m_imageProcessor.flipHorizontally();
    transformImage();
    update();
}

void ImageAreaWidget::onFlipVerticallyTriggered()
{
    m_imageProcessor.flipVertically();
    transformImage();
    update();
}

void ImageAreaWidget::onIncreaseOffsetX(const int pixels)
{
    m_imageProcessor.addImageOffsetX(pixels);
    repaintWithTransformations();
}

void ImageAreaWidget::onIncreaseOffsetY(const int pixels)
{
    m_imageProcessor.addImageOffsetY(pixels);
    repaintWithTransformations();
}

void ImageAreaWidget::onNextImage()
{
    m_originalImage = m_imageLoader.getNextImage();
    m_imageProcessor.bind(m_originalImage, false);
    transformImage();
    update();

    if (const auto delay = m_imageLoader.nextImageDelay(); delay > 0)
        QTimer::singleShot(delay, this, SLOT(onNextImage()));
}

void ImageAreaWidget::onRotateLeftTriggered()
{
    m_imageProcessor.rotateLeft();
    transformImage();
    update();
}

void ImageAreaWidget::onRotateRightTriggered()
{
    m_imageProcessor.rotateRight();
    transformImage();
    update();
}

void ImageAreaWidget::onScrollDownTriggered()
{
    onDecreaseOffsetY();
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

void ImageAreaWidget::onSetFitToWindowTriggered(const bool enabled)
{
    m_imageProcessor.setFitToArea(enabled);
    m_imageProcessor.setScaleFactor(1.0);
    transformImage();
    update();
}

void ImageAreaWidget::zoom(const double factor, bool isZoomIn)
{
    constexpr double maxValue = 2.0;
    constexpr double minValue = 0.1;

    const double newScaleFactor = factor + m_imageProcessor.getScaleFactor();
    if (isZoomIn)
        m_imageProcessor.setScaleFactor(newScaleFactor < maxValue ? newScaleFactor : maxValue);
    else
        m_imageProcessor.setScaleFactor(newScaleFactor > minValue ? newScaleFactor : minValue);

    transformImage();
    update();
}

void ImageAreaWidget::onZoomImageInTriggered(const double factor)
{
    zoom(factor, true);
}

void ImageAreaWidget::onZoomImageOutTriggered(const double factor)
{
    zoom(-factor, false);
}

void ImageAreaWidget::onZoomResetTriggered()
{
    const bool isFitToWindow = m_imageProcessor.isFitToAreaEnabled();
    m_imageProcessor.setFitToArea(false);
    m_imageProcessor.setScaleFactor(1.0);
    transformImage();
    update();
    m_imageProcessor.setFitToArea(isFitToWindow);
}

bool ImageAreaWidget::event(QEvent *ev)
{
    if (ev->type() == QEvent::NativeGesture)
    {
        nativeGestureEvent(dynamic_cast<QNativeGestureEvent *>(ev));
        return ev->isAccepted();
    }

    return QWidget::event(ev);
}

void ImageAreaWidget::gestureZoom(qreal value)
{
    value /= 5;
    if (value >= 0)
        onZoomImageInTriggered(value);
    else
        onZoomImageOutTriggered(-value);
}

void ImageAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_mouseMoveLast.isNull())
    {
        QPoint delta = event->pos() - m_mouseMoveLast;

        // Scroll image only if mouse was moved at least by 10 pixels.
        if (delta.manhattanLength() > 10)
        {
            qDebug() << "MouseMove: " << event->pos() << " prev: " << m_mouseMoveLast << " delta: " << delta;
            m_mouseMoveLast = event->pos();
            scrollTo(delta);
        }
    }

    event->accept();
}

void ImageAreaWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "MousePressed: " << event->pos();
    m_mouseMoveLast = event->pos();
    event->accept();
}

void ImageAreaWidget::nativeGestureEvent(QNativeGestureEvent *event)
{
    static qreal zoomPercentage = 0;

    switch (event->gestureType())
    {
        case Qt::EndNativeGesture:
            if (zoomPercentage != 0)
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
        case Qt::SmartZoomNativeGesture: {
            const double factor = 1000;
            if (event->value() != 0)
                onZoomImageInTriggered(factor);
            else
                onZoomImageOutTriggered(factor);
            break;
        }
        default:; // nothing here - intentionally
    }

    event->accept();
}

void ImageAreaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    const QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, m_finalImage, dirtyRect);
}

void ImageAreaWidget::resizeEvent(QResizeEvent *event)
{
    transformImage();
    update();
    QWidget::resizeEvent(event);
}

void ImageAreaWidget::scrollTo(const QPoint &point)
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

void ImageAreaWidget::transformImage()
{
    if (m_originalImage.isNull())
        return;

    m_imageProcessor.setAreaSize(size());
    m_finalImage = m_imageProcessor.process();

    emit zoomPercentageChanged(m_imageProcessor.getScaleFactor() * m_originalImage.width() / m_originalImage.width());
}

void ImageAreaWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    // High-res input
    if (!numPixels.isNull())
    {
        scrollTo(numPixels);
    } // Low-res input
    else if (!numDegrees.isNull())
    {
        // Zoom
        if (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)
        {
            qDebug() << "Wheel zoom: " << numDegrees.rx() << " - " << numDegrees.ry();
            const int degrees = numDegrees.rx() > 0 || numDegrees.rx() < 0 ? numDegrees.rx() : numDegrees.ry();
            onZoomImageInTriggered(degrees / 325.0);
        }
        // Scroll
        else
        {
            QPoint numSteps = numDegrees / 15;
            numSteps.rx() *= m_imageOffsetStep;
            numSteps.ry() *= m_imageOffsetStep;
            scrollTo(numSteps);
        }
    }

    event->accept();
}

void ImageAreaWidget::extractMetadata(const QString &fileName)
{

    MetadataExtractor metadataExtractor;
    auto connection = connect(&metadataExtractor,
                              &MetadataExtractor::imageInformationParsed,
                              this,
                              [this](const std::vector<std::pair<QString, QString>>& information) {
                                  emit imageInformationParsed(information);
                              });

    auto connectionSize = connect(&metadataExtractor,
                                  &MetadataExtractor::imageSizeParsed,
                                  this,
                                  [this](const uint64_t &size) {
                                      emit imageSizeChanged(size);
                                  });

    metadataExtractor.extract(fileName, m_originalImage.width(), m_originalImage.height());

    disconnect(connection);
    disconnect(connectionSize);
}
