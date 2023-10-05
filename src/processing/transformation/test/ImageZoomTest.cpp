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
    const QSize size{10, 20};
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
    const QSize size{10, 20};
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
    const double scaleFactor = 2;
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
    ImageZoom<QTransform> imageZoom;
    imageZoom.setIsCacheDirty(false);
    imageZoom.resetProperties();
    QCOMPARE(imageZoom.isCacheDirty(), true);
}

void ImageZoomTest::transform() const
{

}
