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

template<typename T>
class ImageZoom : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QImage, U>
    {
        if (ImageTransformationBase<T>::isCacheDirty())
        {
            QImage originalImage = ImageTransformationBase<T>::getOriginalObject();
            ImageTransformationBase<T>::setCachedObject([&originalImage, &m_fitToArea = m_fitToArea, &m_areaWidth = m_areaWidth, &m_areaHeight = m_areaHeight, &m_scaleFactor = m_scaleFactor]() {
                if (m_fitToArea)
                {
                    if ((static_cast<double>(m_areaWidth) / originalImage.width() * originalImage.height()) <= m_areaHeight)
                        return originalImage.scaledToWidth(m_areaWidth, Qt::SmoothTransformation);
                    else
                        return originalImage.scaledToHeight(m_areaHeight, Qt::SmoothTransformation);
                }
                else
                    return originalImage.scaledToWidth((int)(originalImage.width() * m_scaleFactor), Qt::SmoothTransformation);
            }());

            m_scaleFactor = ImageTransformationBase<T>::getCachedObject().width() / static_cast<double>(originalImage.width());
        }
        return ImageTransformationBase<T>::getCachedObject();
    }

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QTransform, U>
    {
        if (ImageTransformationBase<T>::isCacheDirty())
        {
            if (m_fitToArea)
                m_scaleFactor = m_areaWidth / static_cast<double>(m_originalImageSize.width());
            ImageTransformationBase<T>::setCachedObject(QTransform(ImageTransformationBase<T>::getOriginalObject()).scale(m_scaleFactor, m_scaleFactor));
        }
        return ImageTransformationBase<T>::getCachedObject();
    }

    void setOriginalImageSize(const QSize &size);
    void setAreaSize(const QSize &size);
    double getScaleFactor() const;
    void setScaleFactor(double factor);

    bool isFitToAreaEnabled() const;
    void setFitToArea(bool fitToArea);

private:
    int m_areaWidth {0};
    int m_areaHeight {0};
    bool m_fitToArea {false};
    QSize m_originalImageSize {};
    double m_scaleFactor {1.0};
};

template<typename T>
void ImageZoom<T>::setAreaSize(const QSize &size)
{
    if (m_areaHeight == size.height() && m_areaWidth == size.width())
        return;

    m_areaHeight = size.height();
    m_areaWidth = size.width();
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T>
double ImageZoom<T>::getScaleFactor() const
{
    return m_scaleFactor;
}

template<typename T>
void ImageZoom<T>::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T>
bool ImageZoom<T>::isFitToAreaEnabled() const
{
    return m_fitToArea;
}

template<typename T>
void ImageZoom<T>::setFitToArea(bool fitToArea)
{
    m_fitToArea = fitToArea;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T>
void ImageZoom<T>::resetProperties()
{
    m_scaleFactor = 1.0;
    ImageTransformationBase<T>::resetProperties();
}

template<typename T>
QVariant ImageZoom<T>::transform()
{
    return transformInternal<T>();
}

template<typename T>
void ImageZoom<T>::setOriginalImageSize(const QSize &size)
{
    m_originalImageSize = size;
}
