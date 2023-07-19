#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
