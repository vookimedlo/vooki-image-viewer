#pragma once
#include <QImage>
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

#include <QImage>
#include <array>
#include "transformation/ImageBorder.h"
#include "transformation/ImageFlip.h"
#include "transformation/ImageRotation.h"
#include "transformation/ImageZoom.h"
#include "../util/array.h"
#include "../util/compiler.h"
#include "../util/RotatingIndex.h"

class ImageProcessor
{
public:
    ImageProcessor() = default;
    DISABLE_COPY_MOVE(ImageProcessor);


    void bind(const QImage &image, bool resetTransformation = true);

    void flipHorizontally();
    void flipVertically();

    void rotateLeft();
    void rotateRight();

    void resetTransformation() const;

    QImage process();
    void setAreaSize(const QSize &size);

    double getScaleFactor() const;
    void setScaleFactor(double value);

    bool isFitToAreaEnabled() const;
    void setFitToArea(bool fitToArea);

    void addImageOffsetY(int imageOffsetY);
    int getImageOffsetY() const;
    void setImageOffsetY(int imageOffsetY);
    void addImageOffsetX(int imageOffsetX);
    int getImageOffsetX() const;
    void setImageOffsetX(int imageOffsetX);

    void setBorderColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setDrawBorder(bool drawBorder);

protected:
    void flip();

private:
    ImageRotation<QTransform> m_imageRotation {};
    ImageFlip<QTransform> m_imageFlip {};
    ImageZoom<QTransform> m_imageZoom {};
    ImageBorder<QImage> m_imageBorder {};

    constexpr static int m_transformationsSize = 3;
    constexpr static int m_imageTransformationsSize = 1;
    constexpr static int m_genericTransformationsSize = m_transformationsSize + m_imageTransformationsSize;

    const std::array<ImageTransformationBase<QTransform>* const, m_transformationsSize> m_transformations { &m_imageRotation, &m_imageFlip, &m_imageZoom };
    const std::array<ImageTransformationBase<QImage>* const, m_imageTransformationsSize> m_imageTransformations { &m_imageBorder };
    const std::array<ImageTransformation* const, m_genericTransformationsSize> m_genericTransformations { Array::concatenate<ImageTransformation* const>(m_transformations, m_imageTransformations) };
    static_assert(m_transformationsSize > 0, "m_transformations needs to have at least 1 element");
    static_assert(m_imageTransformationsSize > 0, "m_imageTransformations needs to have at least 1 element");
    QImage m_originalImage {};
};
