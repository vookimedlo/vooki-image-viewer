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

#include "ImageTransformation.h"
#include <QColor>


class ImageBorder : public ImageTransformation
{
public:
    void resetProperties() override;
    QImage transform() override;

    void setAreaSize(const QSize &size);
    void setBorderColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    void setDrawBorder(bool drawBorder);
    void addImageOffsetY(int imageOffsetY);
    int getImageOffsetY() const;
    void setImageOffsetY(int imageOffsetY);
    void addImageOffsetX(int imageOffsetX);
    int getImageOffsetX() const;
    void setImageOffsetX(int imageOffsetX);

protected:
    void checkScrollOffset(const QImage &image);

private:
    QSize m_areaSize {};
    QColor m_borderColor { Qt::white };
    QColor m_backgroundColor { Qt::black };
    bool m_drawBorder{ false };
    int m_imageOffsetY {0};
    int m_imageOffsetX {0};
};
