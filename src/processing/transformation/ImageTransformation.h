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

class ImageTransformation
{
public:
    virtual void bind(const QImage &image) { m_originalImage = image; invalidateCache(); };
    virtual QImage transform() = 0;

    [[nodiscard]] inline bool isCacheDirty() const { return m_isCacheDirty; }
    inline void invalidateCache() { m_isCacheDirty = true; }
    inline void setIsCacheDirty(bool isCacheDirty) { m_isCacheDirty = isCacheDirty; }

    virtual void resetProperties() { m_isCacheDirty = true; }

protected:
    inline const QImage &getOriginalImage() const { return m_originalImage; }
    inline const QImage &getCachedImage() const { return m_cachedImage; }
    inline void setCachedImage(const QImage &cachedImage) { m_cachedImage = cachedImage; setIsCacheDirty(false); }

private:
    bool m_isCacheDirty {true};
    QImage m_cachedImage {};
    QImage m_originalImage {};
};
