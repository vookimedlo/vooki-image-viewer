#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
            {
                const auto&& [originalWidth, originalHeight]
                {
                    [&m_originalImageSize = m_originalImageSize, isRotating = ImageTransformationBase<T>::getOriginalObject().isRotating()]() {
                        // Rotation is reported just for 90 and 270 degrees. That's exactly what we want.
                        // See https://doc.qt.io/qt-6/qtransform.html#isRotating
                        if (isRotating)
                            return std::make_pair(m_originalImageSize.height(), m_originalImageSize.width());
                        else
                            return std::make_pair(m_originalImageSize.width(), m_originalImageSize.height());
                    }()
                };

                if (auto scaleFactor {m_areaWidth / static_cast<double>(originalWidth)}; (scaleFactor * originalHeight) < m_areaHeight)
                    m_scaleFactor = scaleFactor;
                else
                    m_scaleFactor = m_areaHeight / static_cast<double>(originalHeight);
            }
            ImageTransformationBase<T>::setCachedObject(QTransform{ImageTransformationBase<T>::getOriginalObject()}.scale(m_scaleFactor, m_scaleFactor));
        }
        return ImageTransformationBase<T>::getCachedObject();
    }

    [[nodiscard]] QSize getOriginalImageSize() const;
    void setOriginalImageSize(const QSize &size);
    [[nodiscard]] QSize getAreaSize() const;
    void setAreaSize(const QSize &size);
    [[nodiscard]] double getScaleFactor() const;
    void setScaleFactor(double factor);
    [[nodiscard]] bool isFitToAreaEnabled() const;
    void setFitToArea(bool fitToArea);

private:
    int m_areaWidth {0};
    int m_areaHeight {0};
    bool m_fitToArea {false};
    QSize m_originalImageSize {};
    double m_scaleFactor {1.0};
};

template<typename T> requires std::is_same_v<QTransform, T>
QSize ImageZoom<T>::getAreaSize() const
{
    return { m_areaWidth, m_areaHeight };
}

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
QSize ImageZoom<T>::getOriginalImageSize() const
{
    return m_originalImageSize;
}

template<typename T> requires std::is_same_v<QTransform, T>
void ImageZoom<T>::setOriginalImageSize(const QSize &size)
{
    m_originalImageSize = size;
    ImageTransformationBase<T>::invalidateCache();
}
