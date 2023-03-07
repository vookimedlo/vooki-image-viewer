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
class ImageFlip : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QTransform, U>
    {
        if (ImageTransformationBase<T>::isCacheDirty())
        {
            QTransform transform {ImageTransformationBase<T>::getOriginalObject()};
            if (m_flipHorizontally)
                transform.rotate(180, Qt::XAxis);

            if (m_flipVertically)
                transform.rotate(180, Qt::YAxis);

            ImageTransformationBase<T>::setCachedObject(transform);
        }

        return ImageTransformationBase<T>::getCachedObject();
    }

    void flipHorizontally();
    void flipVertically();
    bool isFlippedHorizontally() const;
    bool isFlippedVertically() const;
    void setFlipHorizontally(bool flipHorizontally);
    void setFlipVertically(bool flipVertically);

private:
    bool m_flipHorizontally {false};
    bool m_flipVertically {false};
};

template<typename T> requires std::is_same_v<QTransform, T>
void ImageFlip<T>::flipHorizontally()
{
    m_flipHorizontally = !m_flipHorizontally;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageFlip<T>::flipVertically()
{
    m_flipVertically = !m_flipVertically;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
bool ImageFlip<T>::isFlippedHorizontally() const
{
    return m_flipHorizontally;
}

template<typename T> requires std::is_same_v<QTransform, T>
bool ImageFlip<T>::isFlippedVertically() const
{
    return m_flipVertically;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageFlip<T>::setFlipHorizontally(bool flipHorizontally)
{
    m_flipHorizontally = flipHorizontally;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageFlip<T>::setFlipVertically(bool flipVertically)
{
    m_flipVertically = flipVertically;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageFlip<T>::resetProperties()
{
    m_flipHorizontally = m_flipVertically = false;
    ImageTransformationBase<T>::resetProperties();
}

template<typename T> requires std::is_same_v<QTransform, T>
QVariant ImageFlip<T>::transform()
{
    return transformInternal<T>();
}
