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
#include "../util/RotatingIndex.h"


class ImageProcessor
{
public:
    void bind(const QImage &image);

    void flipHorizontally();
    void flipVertically();

    void rotateLeft();
    void rotateRight();

    void resetTransformation();

    QImage process();
    void setAreaSize(const QSize &size);

    double getScaleFactor() const;
    void setScaleFactor(double value);

private:
    bool m_flipHorizontally {false};
    bool m_flipVertically {false};

    int m_areaWidth {0};
    int m_areaHeight {0};
    bool m_fitToArea {false};

public:
    bool isFitToAreaEnabled() const;
    void setFitToArea(bool fitToArea);

private:
    double m_scaleFactor {1.0};

    QImage m_originalImage {};
    RotatingIndex<uint8_t> m_rotateIndex {0, 4}; // 4 rotation quadrants
};
