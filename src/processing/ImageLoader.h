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
#include <QImageReader>
#include <QString>
#include "../util/compiler.h"
#include "../util/RotatingIndex.h"

class ImageLoader
{
public:
    ImageLoader() = default;
    DISABLE_COPY_MOVE(ImageLoader);

    bool loadImage(const QString &fileName);
    const QImage &getImage();
    const QImage &getNextImage();
    bool isAnimated() const;
    int imageCount() const;
    int nextImageDelay() const;

private:
    RotatingIndex<int> m_animationIndex {0, 1};
    QImage m_originalImage {};
    QImageReader m_reader {};

    static constexpr int m_maxAllocationImageSize = 4096;
};
