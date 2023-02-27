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
#include <algorithm>
#include <array>
#include "transformation/ImageBorder.h"
#include "transformation/ImageFlip.h"
#include "transformation/ImageRotation.h"
#include "transformation/ImageZoom.h"
#include "../util/compiler.h"
#include "../util/RotatingIndex.h"

template <class T, std::size_t N, std::size_t... I>
    requires std::is_pointer_v<T> && (N > 0)
constexpr std::array<T, N>
construct_array_impl(const T *a, std::index_sequence<I...>)
{
    return {{a[I]...}};
}

template <class T, std::size_t N>
    requires std::is_pointer_v<T> && (N > 0)
constexpr std::array<T, N> construct_array(const T *source)
{
    return construct_array_impl<T, N>(source, std::make_index_sequence<N>{});
}

template<typename BASE, typename T1, std::size_t N, typename T2, std::size_t M>
    requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
             std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>>
constexpr std::array<BASE, N+M> concat(const std::array<T1, N>&array1, const std::array<T2, M>&array2)
{
    std::array<std::remove_cv_t<BASE>, N+M> result;
    std::copy(array1.cbegin(), array1.cend(), result.begin());
    std::copy(array2.cbegin(), array2.cend(), result.begin() + N);
    return construct_array<BASE, N+M>(result.data());
}

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

    constexpr static int m_transformationsSize = 3;
    constexpr static int m_imageTransformationsSize = 1;
    constexpr static int m_genericTransformationsSize = m_transformationsSize + m_imageTransformationsSize;

    const std::array<ImageTransformationBase<QTransform>* const, m_transformationsSize> m_transformations { &m_imageRotation, &m_imageFlip, &m_imageZoom };
    const std::array<ImageTransformationBase<QImage>* const, m_imageTransformationsSize> m_imageTransformations { &m_imageBorder };
    const std::array<ImageTransformation* const, m_genericTransformationsSize> m_genericTransformations { concat<ImageTransformation* const>(m_transformations, m_imageTransformations) };
    static_assert(m_genericTransformationsSize > 0, "m_transformationsSize needs to have at least 1 element");
    QImage m_originalImage {};
};
