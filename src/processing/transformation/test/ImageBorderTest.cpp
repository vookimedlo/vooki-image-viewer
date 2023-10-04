/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QImage>

#include "ImageBorderTest.h"
#include "../ImageBorder.h"
#include "../../../util/testing.h"

void ImageBorderTest::areaSize() const
{
    ImageBorder<QImage> imageBorder;
    QCOMPARE(imageBorder.getAreaSize(), QSize());

    constexpr QSize size(10, 20);
    imageBorder.setAreaSize(size);
    QCOMPARE(imageBorder.getAreaSize(), size);
}

void ImageBorderTest::backgroundColor() const
{
    ImageBorder<QImage> imageBorder;
    QCOMPARE(imageBorder.getBackgroundColor(), QColor{ Qt::black });

    const QColor color { Qt::darkMagenta };
    imageBorder.setBackgroundColor(color);
    QCOMPARE(imageBorder.getBackgroundColor(), color);
}

void ImageBorderTest::borderColor() const
{
    ImageBorder<QImage> imageBorder;
    QCOMPARE(imageBorder.getBorderColor(), QColor{ Qt::white });

    const QColor color { Qt::green };
    imageBorder.setBorderColor(color);
    QCOMPARE(imageBorder.getBorderColor(), color);
}

void ImageBorderTest::drawBorder() const
{
    ImageBorder<QImage> imageBorder;
    QCOMPARE(imageBorder.getDrawBorder(), false);

    imageBorder.setDrawBorder(true);
    QCOMPARE(imageBorder.getDrawBorder(), true);

    imageBorder.setDrawBorder(false);
    QCOMPARE(imageBorder.getDrawBorder(), false);
}

void ImageBorderTest::imageOffsets() const
{
    ImageBorder<QImage> imageBorder;

    constexpr int initialOffset = 0;
    QCOMPARE(imageBorder.getImageOffsetX(), initialOffset);
    QCOMPARE(imageBorder.getImageOffsetY(), initialOffset);

    constexpr int setInitialOffsetX = 9;
    imageBorder.setImageOffsetX(setInitialOffsetX);
    QCOMPARE(imageBorder.getImageOffsetX(), setInitialOffsetX);
    QCOMPARE(imageBorder.getImageOffsetY(), initialOffset);

    constexpr int setInitialOffsetY = 7;
    imageBorder.setImageOffsetY(setInitialOffsetY);
    QCOMPARE(imageBorder.getImageOffsetY(), setInitialOffsetY);
    QCOMPARE(imageBorder.getImageOffsetX(), setInitialOffsetX);

    constexpr int substraction = -20;
    imageBorder.addImageOffsetX(substraction);
    QCOMPARE(imageBorder.getImageOffsetX(), setInitialOffsetX + substraction);
    QCOMPARE(imageBorder.getImageOffsetY(), setInitialOffsetY);

    imageBorder.addImageOffsetY(substraction);
    QCOMPARE(imageBorder.getImageOffsetY(), setInitialOffsetY + substraction);
    QCOMPARE(imageBorder.getImageOffsetX(), setInitialOffsetX + substraction);
}

void ImageBorderTest::resetProperties() const
{
    ImageBorder<QImage> imageBorder;

    constexpr int initialOffset = 0;

    constexpr int setInitialOffset = 9;
    imageBorder.setImageOffsetX(setInitialOffset);
    imageBorder.setImageOffsetY(setInitialOffset);

    imageBorder.setDrawBorder(true);

    const QColor color { Qt::green };
    imageBorder.setBorderColor(color);
    imageBorder.setBackgroundColor(color);

    constexpr QSize size(10, 20);
    imageBorder.setAreaSize(size);

    imageBorder.resetProperties();
    QCOMPARE(imageBorder.getImageOffsetX(), initialOffset);
    QCOMPARE(imageBorder.getImageOffsetY(), initialOffset);

    // Only the offset shall be reset, the rest shall be kept.
    QCOMPARE(imageBorder.getDrawBorder(), true);
    QCOMPARE(imageBorder.getBorderColor(), color);
    QCOMPARE(imageBorder.getBackgroundColor(), color);
    QCOMPARE(imageBorder.getAreaSize(), size);
}
