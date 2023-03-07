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

#include <QImage>
#include <QVariant>
#include <type_traits>
#include "ImageTransformation.h"

template<typename T> requires std::is_same_v<QImage, T> || std::is_same_v<QTransform, T>
class ImageTransformationBase : public ImageTransformation
{
public:
    template<typename U = T>
    inline void invalidateCache() requires std::is_same_v<QImage, U> { setIsCacheDirty(true); }

    template<typename U = T>
    inline void invalidateCache() requires std::is_same_v<QTransform, U> { setIsCacheDirty(true); m_cachedObject.reset(); }

    virtual void bind(const T &object) {
        m_originalObject = object;
        invalidateCache<T>();
    };

protected:
    [[nodiscard]] inline const T &getOriginalObject() const { return m_originalObject; }
    [[nodiscard]] inline const T &getCachedObject() const { return m_cachedObject; }

    inline void setCachedObject(const T &object) {
        m_cachedObject = object;
        setIsCacheDirty(false);
    }

private:
    T m_cachedObject {};
    T m_originalObject {};
};
