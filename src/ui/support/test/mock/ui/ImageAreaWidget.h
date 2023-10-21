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


class ImageAreaWidget final : public QWidget
{
    Q_OBJECT

    static constexpr int m_imageOffsetStep {0};

public:
    using QWidget::QWidget;
    DISABLE_COPY_MOVE(ImageAreaWidget);

signals:
    void imageInformationParsed(const std::vector<std::pair<QString, QString>>& information);
    void imageDimensionsChanged(int width, int height);
    void imageSizeChanged(uint64_t size);
    void zoomPercentageChanged(qreal value);

public slots:
    void onDecreaseOffsetX([[maybe_unused]] int pixels = m_imageOffsetStep) const {};
    void onDecreaseOffsetY([[maybe_unused]] int pixels = m_imageOffsetStep) const {};
    void onFlipHorizontallyTriggered() const {};
    void onFlipVerticallyTriggered() const {};
    void onIncreaseOffsetY([[maybe_unused]] int pixels = m_imageOffsetStep) const {};
    void onIncreaseOffsetX([[maybe_unused]] int pixels = m_imageOffsetStep) const {};
    void onNextImage() const {};
    void onRotateLeftTriggered() const {};
    void onRotateRightTriggered() const {};
    void onScrollDownTriggered() const {};
    void onScrollLeftTriggered() const {};
    void onScrollRightTriggered() const {};
    void onScrollUpTriggered() const {};
    void onSetFitToWindowTriggered([[maybe_unused]] bool enabled) const {};
    void onZoomImageInTriggered([[maybe_unused]] double factor) const {};
    void onZoomImageOutTriggered([[maybe_unused]] double factor) const {};
    void onZoomResetTriggered() const {};
};
