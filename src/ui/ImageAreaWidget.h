#pragma once
/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

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

#include <cstdint>
#include <QColor>
#include <QWidget>
#include "../util/compiler.h"
#include "../util/RotatingIndex.h"

class ImageAreaWidget : public QWidget
{
    Q_OBJECT

public:
    ImageAreaWidget(QWidget *parent = 0);
    DISABLE_COPY_MOVE(ImageAreaWidget);

    void drawBorder(bool draw, const QColor &color = QColor(Qt::white));
    void flipHorizontally();
    void flipVertically();
    void setFitToWindow(bool enabled);
    bool showImage(const QString &fileName);
    void repaintWithTransformations();
    void rotateLeft();
    void rotateRight();
    void zoomImageIn(double factor);
    void zoomImageOut(double factor);
    void zoomReset();

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void transformImage();

private:
    bool m_drawBorder;
    bool m_flipHorizontally;
    bool m_flipVertically;
    bool m_isFitToWindow;
    double m_scaleFactor;
    QColor m_borderColor;
    QImage m_originalImage;
    QImage m_finalImage;
    RotatingIndex<uint8_t> m_rotateIndex;
};
