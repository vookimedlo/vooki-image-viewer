#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
    [[nodiscard]] const QImage &getImage();
    [[nodiscard]] const QImage &getNextImage();
    [[nodiscard]] bool isAnimated() const;
    [[nodiscard]] int imageCount() const;
    [[nodiscard]] int nextImageDelay() const;

private:
    RotatingIndex<> m_animationIndex {0, 1};
    QImage m_originalImage {};
    QImageReader m_reader {};

    static constexpr int m_maxAllocationImageSize = 4096;
};
