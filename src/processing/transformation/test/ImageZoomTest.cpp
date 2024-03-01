/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QImage>

#include "ImageZoomTest.h"
#include "../ImageZoom.h"
#include "../../../util/testing.h"

void ImageZoomTest::areaSize() const
{
    constexpr QSize size{10, 20};
    ImageZoom<QTransform> imageZoom;
    QCOMPARE(imageZoom.getAreaSize(), QSize(0, 0));
    QCOMPARE(imageZoom.isCacheDirty(), true);
    imageZoom.setIsCacheDirty(true);
    imageZoom.setAreaSize(size);
    QCOMPARE(imageZoom.isCacheDirty(), true);
    QCOMPARE(imageZoom.getAreaSize(), size);

    // Check the cache dirtiness is not changed when setting the identical size.
    imageZoom.setIsCacheDirty(false);
    imageZoom.setAreaSize(size);
    QCOMPARE(imageZoom.isCacheDirty(), false);
}

void ImageZoomTest::fitToArea() const
{
    ImageZoom<QTransform> imageZoom;
    QCOMPARE(imageZoom.isCacheDirty(), true);
    QCOMPARE(imageZoom.isFitToAreaEnabled(), false);
    imageZoom.setIsCacheDirty(false);
    imageZoom.setFitToArea(true);
    QCOMPARE(imageZoom.isFitToAreaEnabled(), true);
    QCOMPARE(imageZoom.isCacheDirty(), true);
    imageZoom.setFitToArea(false);
    QCOMPARE(imageZoom.isFitToAreaEnabled(), false);
}

void ImageZoomTest::originalImageSize() const
{
    constexpr QSize size{10, 20};
    ImageZoom<QTransform> imageZoom;
    QCOMPARE(imageZoom.isCacheDirty(), true);
    QCOMPARE(imageZoom.getOriginalImageSize(), QSize());
    imageZoom.setIsCacheDirty(false);
    imageZoom.setOriginalImageSize(size);
    QCOMPARE(imageZoom.getOriginalImageSize(), size);
    QCOMPARE(imageZoom.isCacheDirty(), true);
}

void ImageZoomTest::scaleFactor() const
{
    constexpr double scaleFactor = 2;
    ImageZoom<QTransform> imageZoom;
    QCOMPARE(imageZoom.isCacheDirty(), true);
    QCOMPARE(imageZoom.getScaleFactor(), 1);
    imageZoom.setIsCacheDirty(false);
    imageZoom.setScaleFactor(scaleFactor);
    QCOMPARE(imageZoom.getScaleFactor(), scaleFactor);
    QCOMPARE(imageZoom.isCacheDirty(), true);
}

void ImageZoomTest::resetProperties() const
{
    constexpr QSize size{10, 20};
    ImageZoom<QTransform> imageZoom;
    imageZoom.setIsCacheDirty(false);
    imageZoom.setAreaSize(size);
    imageZoom.setFitToArea(true);
    imageZoom.setOriginalImageSize(size);
    imageZoom.setScaleFactor(8.3);

    imageZoom.resetProperties();

    QCOMPARE(imageZoom.getAreaSize(), size);
    QCOMPARE(imageZoom.getOriginalImageSize(), size);
    QCOMPARE(imageZoom.getScaleFactor(), 1);
    QCOMPARE(imageZoom.isFitToAreaEnabled(), true);
    QCOMPARE(imageZoom.isCacheDirty(), true);
}

void ImageZoomTest::transform() const
{
    {
        ImageZoom<QTransform> imageZoom{};
        auto transformBefore = imageZoom.transform().value<QTransform>();
        QCOMPARE(imageZoom.transform().value<QTransform>(), transformBefore);
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(1, 1));
    }

    {
        ImageZoom<QTransform> imageZoom{};
        imageZoom.setScaleFactor(2.5);
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(2.5, 2.5));
    }

    {
        ImageZoom<QTransform> imageZoom{};
        imageZoom.setScaleFactor(11111111.1);
        imageZoom.setFitToArea(true);
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(0, 0));
    }

    {
        ImageZoom<QTransform> imageZoom{};
        imageZoom.setScaleFactor(1111111.1);
        imageZoom.setFitToArea(true);
        imageZoom.setAreaSize({200, 100});
        imageZoom.setOriginalImageSize({100, 10});
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(2, 2));
    }

    {
        ImageZoom<QTransform> imageZoom{};
        imageZoom.setFitToArea(true);
        imageZoom.setAreaSize({200, 100});
        imageZoom.setOriginalImageSize({100, 10});
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(2, 2));
    }

    {
        ImageZoom<QTransform> imageZoom{};
        imageZoom.setFitToArea(true);
        imageZoom.setAreaSize({20000, 100});
        imageZoom.setOriginalImageSize({100, 10});
        QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().scale(10, 10));
    }

    {
        for (int i = 0; i < 10; ++i)
        {
            QTransform transformation;
            transformation.rotate(90 + 180 * i);
            ImageZoom<QTransform> imageZoom{};
            imageZoom.bind(transformation);
            imageZoom.setFitToArea(true);
            imageZoom.setAreaSize({ 200, 150 });
            imageZoom.setOriginalImageSize({ 100, 10 });
            QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().rotate(90 + 180 * i).scale(1.5, 1.5));
        }
    }

    {
        for (int i = 0; i < 10; ++i)
        {
            QTransform transformation;
            transformation.rotate(90 + 180 * i);
            ImageZoom<QTransform> imageZoom{};
            imageZoom.bind(transformation);
            imageZoom.setFitToArea(true);
            imageZoom.setAreaSize({ 200, 100 });
            imageZoom.setOriginalImageSize({ 10, 80 });
            QCOMPARE(imageZoom.transform().value<QTransform>(), QTransform().rotate(90 + 180 * i).scale(2.5, 2.5));
        }
    }
}
