#pragma once
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

#include "ImageTransformationBase.h"
#include <QColor>
#include <QPainter>


template<typename T> requires std::is_same_v<QImage, T>
class ImageBorder : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    void setAreaSize(const QSize &size);
    void setBorderColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setDrawBorder(bool drawBorder);
    void addImageOffsetY(int imageOffsetY);
    int getImageOffsetY() const;
    void setImageOffsetY(int imageOffsetY);
    void addImageOffsetX(int imageOffsetX);
    int getImageOffsetX() const;
    void setImageOffsetX(int imageOffsetX);

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
void ImageBorder<T>::setAreaSize(const QSize &size)
{
    m_areaSize = size;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QImage, T>
void ImageBorder<T>::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    ImageTransformationBase<T>::invalidateCache();
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
        QImage newImage {newSize, QImage::Format_RGB32};
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
            painterImage.setBrush(Qt::NoBrush);
            QPen pen = painterImage.pen();
            pen.setWidth(3);
            pen.setColor(m_borderColor);
            painterImage.setPen(pen);
            painterImage.drawRect(x - m_imageOffsetX,
                                  y - m_imageOffsetY,
                                  originalImage.width(),
                                  originalImage.height());
        }

        ImageTransformationBase<T>::setCachedObject(newImage);
    }

    return ImageTransformationBase<T>::getCachedObject();
}
