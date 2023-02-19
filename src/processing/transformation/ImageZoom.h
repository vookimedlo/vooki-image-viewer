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

template<typename T> requires std::is_same_v<QTransform, T>
class ImageZoom : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

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

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::setAreaSize(const QSize &size)
{
    if (m_areaHeight == size.height() && m_areaWidth == size.width())
        return;

    m_areaHeight = size.height();
    m_areaWidth = size.width();
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
double ImageZoom<T>::getScaleFactor() const
{
    return m_scaleFactor;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::setScaleFactor(double factor)
{
    m_scaleFactor = factor;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
bool ImageZoom<T>::isFitToAreaEnabled() const
{
    return m_fitToArea;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::setFitToArea(bool fitToArea)
{
    m_fitToArea = fitToArea;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::resetProperties()
{
    m_scaleFactor = 1.0;
    ImageTransformationBase<T>::resetProperties();
}

template<typename T> requires std::is_same_v<QTransform, T>
QVariant ImageZoom<T>::transform()
{
    return transformInternal<T>();
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::setOriginalImageSize(const QSize &size)
{
    m_originalImageSize = size;
}
