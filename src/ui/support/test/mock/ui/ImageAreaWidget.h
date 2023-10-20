#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../../../../util/compiler.h"
#include <QWidget>
#include <cstdint>
#include <list>
#include <utility>
#include <vector>


class ImageAreaWidget : public QWidget
{
    Q_OBJECT

    static constexpr int m_imageOffsetStep {0};

public:
    explicit ImageAreaWidget(QWidget *parent = nullptr) : QWidget(parent) {};
    DISABLE_COPY_MOVE(ImageAreaWidget);

signals:
    void imageInformationParsed(const std::vector<std::pair<QString, QString>>& information);
    void imageDimensionsChanged(int width, int height);
    void imageSizeChanged(uint64_t size);
    void zoomPercentageChanged(qreal value);

public slots:
    void onDecreaseOffsetX([[maybe_unused]] int pixels = m_imageOffsetStep) {};
    void onDecreaseOffsetY([[maybe_unused]] int pixels = m_imageOffsetStep) {};
    void onFlipHorizontallyTriggered() {};
    void onFlipVerticallyTriggered() {};
    void onIncreaseOffsetY([[maybe_unused]] int pixels = m_imageOffsetStep) {};
    void onIncreaseOffsetX([[maybe_unused]] int pixels = m_imageOffsetStep) {};
    void onNextImage() {};
    void onRotateLeftTriggered() {};
    void onRotateRightTriggered() {};
    void onScrollDownTriggered() {};
    void onScrollLeftTriggered() {};
    void onScrollRightTriggered() {};
    void onScrollUpTriggered() {};
    void onSetFitToWindowTriggered([[maybe_unused]] bool enabled) {};
    void onZoomImageInTriggered([[maybe_unused]] double factor) {};
    void onZoomImageOutTriggered([[maybe_unused]] double factor) {};
    void onZoomResetTriggered() {};
};
