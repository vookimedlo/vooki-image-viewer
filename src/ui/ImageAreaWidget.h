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

#include "../util/RotatingIndex.h"
#include "../util/compiler.h"
#include <QColor>
#include <QWidget>
#include <cstdint>

// Forward declarations
class QNativeGestureEvent;

class ImageAreaWidget : public QWidget
{
    Q_OBJECT
    typedef QWidget BaseClass;

public:
    ImageAreaWidget(QWidget *parent = 0);
    DISABLE_COPY_MOVE(ImageAreaWidget);

    void drawBorder(bool draw, const QColor &color = QColor(Qt::white));
    bool showImage(const QString &fileName);
    void repaintWithTransformations();

signals:
    void zoomPercentageChanged(qreal value);

public slots:
    void onDecreaseOffsetX(int pixels = m_imageOffsetStep);
    void onDecreaseOffsetY(int pixels = m_imageOffsetStep);
    void onFlipHorizontallyTriggered();
    void onFlipVerticallyTriggered();
    void onIncreaseOffsetY(int pixels = m_imageOffsetStep);
    void onIncreaseOffsetX(int pixels = m_imageOffsetStep);
    void onRotateLeftTriggered();
    void onRotateRightTriggered();
    void onScrollDownTriggered();
    void onScrollLeftTriggered();
    void onScrollRightTriggered();
    void onScrollUpTriggered();
    void onSetFitToWindowTriggered(bool enabled);
    void onZoomImageInTriggered(double factor);
    void onZoomImageOutTriggered(double factor);
    void onZoomResetTriggered();

protected:
    void checkScrollOffset();
    bool event(QEvent *ev) override;
    void gestureZoom(qreal value);
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void nativeGestureEvent(QNativeGestureEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void scroll(const QPoint &point);
    void transformImage();
    void wheelEvent(QWheelEvent *event) override;

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
    int m_imageOffsetY;
    int m_imageOffsetX;
    QPoint m_mouseMoveLast;

    static const int m_imageOffsetStep;
};
