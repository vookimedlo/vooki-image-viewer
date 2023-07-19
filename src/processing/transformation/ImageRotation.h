#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../util/RotatingIndex.h"
#include "ImageTransformationBase.h"

template<typename T> requires std::is_same_v<QTransform, T>
class ImageRotation : public ImageTransformationBase<T>
{
public:
    void resetProperties() override;
    QVariant transform() override;

    template<typename U = T>
    QVariant transformInternal() requires std::is_same_v<QTransform, U>
    {
        if (ImageTransformationBase<T>::isCacheDirty())
            ImageTransformationBase<T>::setCachedObject(QTransform{ImageTransformationBase<T>::getOriginalObject()}.rotate(m_rotateIndex * 90));
        return ImageTransformationBase<T>::getCachedObject();
    }

    void rotateLeft();
    void rotateRight();
private:
    RotatingIndex<uint8_t> m_rotateIndex {0, 4}; // 4 rotation quadrants
};

template<typename T> requires std::is_same_v<QTransform, T>
void ImageRotation<T>::rotateLeft()
{
    --m_rotateIndex;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageRotation<T>::rotateRight()
{
    ++m_rotateIndex;
    ImageTransformationBase<T>::invalidateCache();
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageRotation<T>::resetProperties()
{
    m_rotateIndex.reset(0);
    ImageTransformationBase<T>::resetProperties();
}

template<typename T> requires std::is_same_v<QTransform, T>
QVariant ImageRotation<T>::transform()
{
    return transformInternal<T>();
}
