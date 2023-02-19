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

    void resetTransformation();

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
    const std::array<ImageTransformation<QTransform>* const, 3> m_transformations { &m_imageRotation, &m_imageFlip, &m_imageZoom };
    QImage m_originalImage {};
};
