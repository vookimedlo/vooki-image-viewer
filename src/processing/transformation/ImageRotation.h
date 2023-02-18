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
#include "../../util/RotatingIndex.h"


template<typename T>
class ImageRotation : public ImageTransformation<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QImage, U>
    {
        if (ImageTransformation<T>::isCacheDirty())
            ImageTransformation<T>::setCachedObject(ImageTransformation<T>::getOriginalObject().transformed(QTransform().rotate(m_rotateIndex * 90), Qt::SmoothTransformation));
        return ImageTransformation<T>::getCachedObject();
    }

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QTransform, U>
    {
        if (ImageTransformation<T>::isCacheDirty())
            ImageTransformation<T>::setCachedObject(QTransform(ImageTransformation<T>::getOriginalObject()).rotate(m_rotateIndex * 90));
        return ImageTransformation<T>::getCachedObject();
    }

    void rotateLeft();
    void rotateRight();
private:
    RotatingIndex<uint8_t> m_rotateIndex {0, 4}; // 4 rotation quadrants
};

template<typename T>
void ImageRotation<T>::rotateLeft()
{
    --m_rotateIndex;
    ImageTransformation<T>::invalidateCache();
}

template<typename T>
void ImageRotation<T>::rotateRight()
{
    ++m_rotateIndex;
    ImageTransformation<T>::invalidateCache();
}

template<typename T>
void ImageRotation<T>::resetProperties()
{
    m_rotateIndex.reset(0);
    ImageTransformation<T>::resetProperties();
}

template<typename T>
QVariant ImageRotation<T>::transform()
{
    return transformInternal<T>();
}