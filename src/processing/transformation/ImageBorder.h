#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageTransformationBase.h"
#include <QColor>
#include <QPainter>


template<typename T> requires std::is_same_v<QImage, T>
class ImageBorder : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    [[nodiscard]] const QSize &getAreaSize() const;
    void setAreaSize(const QSize &size);
    [[nodiscard]] const QColor &getBorderColor() const;
    void setBorderColor(const QColor &color);
    [[nodiscard]] const QColor &getBackgroundColor() const;
    void setBackgroundColor(const QColor &color);
    [[nodiscard]] bool getDrawBorder() const;
    void setDrawBorder(bool drawBorder);
    void addImageOffsetY(int imageOffsetY);
    [[nodiscard]] int getImageOffsetY() const;
    void setImageOffsetY(int imageOffsetY);
    void addImageOffsetX(int imageOffsetX);
    [[nodiscard]] int getImageOffsetX() const;
    void setImageOffsetX(int imageOffsetX);

    static const int borderWidth {3};
    static const auto format {QImage::Format_RGB32};

protected:
    void checkScrollOffset(const QImage &image);

private:
    QSize m_areaSize {};
    QColor m_borderColor { Qt::white };
    QColor m_backgroundColor { Qt::black };
    bool m_drawBorder{ false };
    int m_imageOffsetY {0};
    int m_imageOffsetX {0};
};

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::checkScrollOffset(const QImage &image)
{
    if (image.height() < m_areaSize.height())
        m_imageOffsetY = 0;
    else if (image.height() - m_imageOffsetY < m_areaSize.height())
        m_imageOffsetY = image.height() - m_areaSize.height();

    if (image.width() < m_areaSize.width())
        m_imageOffsetX = 0;
    else if (image.width() - m_imageOffsetX < m_areaSize.width())
        m_imageOffsetX = image.width() - m_areaSize.width();

    if (m_imageOffsetY < 0)
        m_imageOffsetY = 0;

    if (m_imageOffsetX < 0)
        m_imageOffsetX = 0;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::addImageOffsetY(int imageOffsetY)
{
    m_imageOffsetY += imageOffsetY;
}

template<typename T> requires std::is_same_v<QImage, T>
int ImageBorder<T>::getImageOffsetY() const
{
    return m_imageOffsetY;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setImageOffsetY(int imageOffsetY)
{
    m_imageOffsetY = imageOffsetY;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::addImageOffsetX(int imageOffsetX)
{
    m_imageOffsetX += imageOffsetX;
}

template<typename T> requires std::is_same_v<QImage, T>
int ImageBorder<T>::getImageOffsetX() const
{
    return m_imageOffsetX;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setImageOffsetX(int imageOffsetX)
{
    m_imageOffsetX = imageOffsetX;
}

template<typename T> requires std::is_same_v<QImage, T>
const QSize &ImageBorder<T>::getAreaSize() const
{
    return m_areaSize;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setAreaSize(const QSize &size)
{
    m_areaSize = size;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
const QColor &ImageBorder<T>::getBorderColor() const
{
    return m_borderColor;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
const QColor &ImageBorder<T>::getBackgroundColor() const
{
    return m_backgroundColor;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
bool ImageBorder<T>::getDrawBorder() const
{
    return m_drawBorder;
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setDrawBorder(bool drawBorder)
{
    ImageBorder::m_drawBorder = drawBorder;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::resetProperties()
{
    m_imageOffsetX = m_imageOffsetY = 0;
    ImageTransformationBase<T>::resetProperties();
}

template<typename T> requires std::is_same_v<QImage, T>
QVariant ImageBorder<T>::transform()
{
    if (ImageTransformationBase<T>::isCacheDirty())
    {
        QImage originalImage {ImageTransformationBase<T>::getOriginalObject()};
        QSize newSize {originalImage.size().expandedTo(m_areaSize)};
        QImage newImage {newSize, ImageBorder::format};
        newImage.fill(m_backgroundColor);

        const auto x {newSize.width() / 2 - originalImage.width() / 2};
        const auto y {newSize.height() / 2 - originalImage.height() / 2};

        // Update scroll settings
        checkScrollOffset(newImage);
        QPainter painterImage(&newImage);
        painterImage.drawImage(x,
                               y,
                               originalImage,
                               m_imageOffsetX,
                               m_imageOffsetY);

        if (m_drawBorder)
        {
            constexpr int penWidth = 1;
            QPen pen = painterImage.pen();
            pen.setWidth(penWidth);
            pen.setColor(m_borderColor);
            pen.setStyle(Qt::SolidLine);
            painterImage.setPen(pen);
            painterImage.setBrush(Qt::NoBrush);

            for (int i = 0; i < ImageBorder::borderWidth; ++i)
            {
                painterImage.drawRect(QRect(QPoint{std::max(0, x - m_imageOffsetX + i),
                                                    std::max(0, y - m_imageOffsetY + i)},
                                            QPoint{originalImage.width() + x - m_imageOffsetX - i - penWidth - 1,
                                                    originalImage.height() + y - m_imageOffsetY - i - penWidth - 1}));
            }
        }

        ImageTransformationBase<T>::setCachedObject(newImage);
    }

    return ImageTransformationBase<T>::getCachedObject();
}
