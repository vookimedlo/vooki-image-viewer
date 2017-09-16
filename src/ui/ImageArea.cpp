#include "ImageArea.h"

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>

ImageArea::ImageArea(QWidget *parent): QWidget(parent), isFitToWindow(false), scaleFactor(1.0), originalImage(), scaledImage(), finalImage()
{
    originalImage.fill(qRgb(0, 0, 0));
    scaledImage.fill(qRgb(0, 0, 0));
    finalImage.fill(qRgb(0, 0, 0));
}

bool ImageArea::showImage(const QString &fileName)
{
    if (!originalImage.load(fileName))
        return false;
    scaleFactor = 1;
    scaleImage();
    update();
    return true;
}

void ImageArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, finalImage, dirtyRect);
}

void ImageArea::resizeEvent(QResizeEvent *event)
{
    scaleImage();
    update();
    QWidget::resizeEvent(event);
}

void ImageArea::scaleImage()
{
    if(isFitToWindow)
    {
        //QSize newSize = originalImage.size().expandedTo(size());
        if(originalImage.width() < originalImage.height())
            scaledImage = originalImage.scaledToHeight(height());
        else
            scaledImage = originalImage.scaledToWidth(width());
    }
    else
        scaledImage = originalImage.scaledToWidth(originalImage.width() * scaleFactor);

    QSize newSize = scaledImage.size().expandedTo(size());
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(0, 0, 0));
    QPainter painterImage(&newImage);
    painterImage.drawImage(QPoint(newSize.width() / 2 - scaledImage.size().width() / 2, newSize.height() / 2 - scaledImage.size().height() / 2), scaledImage);
    finalImage = newImage;
}

void ImageArea::setFitToWindow(bool enabled)
{
    isFitToWindow = enabled;
    scaleFactor = 1;
    scaleImage();
    update();
}

void ImageArea::zoomImageIn(double factor)
{
    double newScaleFactor = factor + scaleFactor;
    if(newScaleFactor > 3.0)
        return;

    scaleFactor = newScaleFactor;
    scaleImage();
    update();
}

void ImageArea::zoomImageOut(double factor)
{
    double newScaleFactor = -1 * factor + scaleFactor;
    if(newScaleFactor < 0.2)
        return;

    scaleFactor = newScaleFactor;
    scaleImage();
    update();
}

void ImageArea::zoomReset()
{
    bool isFitToWindow = this->isFitToWindow;
    this->isFitToWindow = false;
    scaleFactor = 1;
    scaleImage();
    update();
    this->isFitToWindow = isFitToWindow;
}
