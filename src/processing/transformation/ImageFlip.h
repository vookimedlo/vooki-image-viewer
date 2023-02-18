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

#include "ImageTransformation.h"

template<typename T>
class ImageFlip : public ImageTransformation<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QImage, U>
    {
        if (ImageTransformation<T>::isCacheDirty())
        {
            QImage newImage = ImageTransformation<T>::getOriginalObject();
            if (m_flipHorizontally)
            {
                static QTransform transform{QTransform().rotate(180, Qt::XAxis)};
                newImage = newImage.transformed(transform, Qt::SmoothTransformation);
            }

            if (m_flipVertically)
            {
                static QTransform transform{QTransform().rotate(180, Qt::YAxis)};
                newImage = newImage.transformed(transform, Qt::SmoothTransformation);
            }

            ImageTransformation<T>::setCachedObject(newImage);
        }

        return ImageTransformation<T>::getCachedObject();
    }

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QTransform, U>
    {
        if (ImageTransformation<T>::isCacheDirty())
        {
            QTransform transform = ImageTransformation<T>::getOriginalObject();
            if (m_flipHorizontally)
                transform.rotate(180, Qt::XAxis);

            if (m_flipVertically)
                transform.rotate(180, Qt::YAxis);

            ImageTransformation<T>::setCachedObject(transform);
        }

        return ImageTransformation<T>::getCachedObject();
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

template<typename T>
void ImageFlip<T>::flipHorizontally()
{
    m_flipHorizontally = !m_flipHorizontally;
    ImageTransformation<T>::invalidateCache();
}

template<typename T>
void ImageFlip<T>::flipVertically()
{
    m_flipVertically = !m_flipVertically;
    ImageTransformation<T>::invalidateCache();
}

template<typename T>
bool ImageFlip<T>::isFlippedHorizontally() const
{
    return m_flipHorizontally;
}

template<typename T>
bool ImageFlip<T>::isFlippedVertically() const
{
    return m_flipVertically;
}

template<typename T>
void ImageFlip<T>::setFlipHorizontally(bool flipHorizontally)
{
    m_flipHorizontally = flipHorizontally;
}

template<typename T>
void ImageFlip<T>::setFlipVertically(bool flipVertically)
{
    m_flipVertically = flipVertically;
}

template<typename T>
void ImageFlip<T>::resetProperties()
{
    m_flipHorizontally = m_flipVertically = false;
    ImageTransformation<T>::resetProperties();
}

template<typename T>
QVariant ImageFlip<T>::transform()
{
    return transformInternal<T>();
}
