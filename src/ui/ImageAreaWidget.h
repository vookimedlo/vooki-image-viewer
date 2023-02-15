#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include <QColor>
#include <QTimer>
#include <QWidget>
#include <cstdint>
#include <list>
#include <qcorotask.h>
#include <utility>
#include <vector>
#include "../processing/ImageLoader.h"
#include "../processing/ImageProcessor.h"
#include "../util/RotatingIndex.h"
#include "../util/compiler.h"

// Forward declarations
class QNativeGestureEvent;

namespace Exiv2
{
    class Image;
}

class ImageAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageAreaWidget(QWidget *parent = nullptr);
    DISABLE_COPY_MOVE(ImageAreaWidget);
    ~ImageAreaWidget() noexcept override;

    void setBackgroundColor(const QColor &color);
    void drawBorder(bool draw, const QColor &color = QColor(Qt::white));
    QCoro::Task<bool> showImage(const QString &fileName);
    void repaintWithTransformations();

signals:
    void imageInformationParsed(const std::vector<std::pair<QString, QString>>& information);
    void imageDimensionsChanged(int width, int height);
    void imageSizeChanged(uint64_t size);
    void zoomPercentageChanged(qreal value);

public slots:
    void onDecreaseOffsetX(int pixels = m_imageOffsetStep);
    void onDecreaseOffsetY(int pixels = m_imageOffsetStep);
    void onFlipHorizontallyTriggered();
    void onFlipVerticallyTriggered();
    void onIncreaseOffsetY(int pixels = m_imageOffsetStep);
    void onIncreaseOffsetX(int pixels = m_imageOffsetStep);
    void onNextImage();
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
    void scrollTo(const QPoint &point);
    void transformImage();
    void wheelEvent(QWheelEvent *event) override;
    void zoom(const double factor, bool isZoomIn);

    void extractMetadata(const QString &fileName);

private:
    QImage m_originalImage {};
    QImage m_finalImage {};
    QPoint m_mouseMoveLast {};
    QTimer m_animationTimer {this};
    ImageLoader m_imageLoader {};
    ImageProcessor m_imageProcessor {};

    static constexpr int m_imageOffsetStep = 100;
};
