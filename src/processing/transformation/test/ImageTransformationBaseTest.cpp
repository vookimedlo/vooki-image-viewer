/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QImage>
#include <QTransform>

#include "ImageTransformationBaseTest.h"
#include "../ImageTransformationBase.h"

template<typename T> requires std::is_same_v<QImage, T> || std::is_same_v<QTransform, T>
final class ImageTransformationBaseHelper : public ImageTransformationBase<T>
{
public:
    [[nodiscard]] const T &getOriginalObject() const { return ImageTransformationBase<T>::getOriginalObject(); }
    [[nodiscard]] const T &getCachedObject() const { return ImageTransformationBase<T>::getCachedObject(); }

    void setCachedObject(const T &object) {
        ImageTransformationBase<T>::setCachedObject(object);
    }

    QVariant transform() override { return QVariant(); }
};

void ImageTransformationBaseTest::setCachedObjectTransform() const
{
    QTransform transformation;
    transformation.rotate(90);

    ImageTransformationBaseHelper<QTransform> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(transformation);
    QCOMPARE(helper.getCachedObject(), transformation);
    QCOMPARE(helper.isCacheDirty(), false);
}

void ImageTransformationBaseTest::setCachedObjectImage() const
{
    QImage image(10, 10, QImage::Format_ARGB32);
    image.setPixel(0, 0, 0xAAFF00);

    ImageTransformationBaseHelper<QImage> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(image);
    QCOMPARE(helper.getCachedObject(), image);
    QCOMPARE(helper.isCacheDirty(), false);
}

void ImageTransformationBaseTest::invalidateCacheTransform() const
{
    QTransform transformation;
    transformation.rotate(-90);

    ImageTransformationBaseHelper<QTransform> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(transformation);
    QCOMPARE(helper.getCachedObject(), transformation);
    QCOMPARE(helper.isCacheDirty(), false);

    helper.invalidateCache();
    QCOMPARE(helper.getCachedObject(), QTransform());
    QCOMPARE(helper.isCacheDirty(), true);
}

void ImageTransformationBaseTest::invalidateCacheImage() const
{
    QImage image(10, 10, QImage::Format_ARGB32);
    image.setPixel(0, 0, 0xAAFF00);

    ImageTransformationBaseHelper<QImage> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(image);
    QCOMPARE(helper.getCachedObject(), image);
    QCOMPARE(helper.isCacheDirty(), false);

    helper.invalidateCache();
    QCOMPARE(helper.getCachedObject(), image);
    QCOMPARE(helper.isCacheDirty(), true);
}

void ImageTransformationBaseTest::bindTransform() const
{
    QTransform transformation;
    transformation.rotate(-90);

    ImageTransformationBaseHelper<QTransform> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(transformation);
    QCOMPARE(helper.getCachedObject(), transformation);
    QCOMPARE(helper.isCacheDirty(), false);

    QTransform boundTransformation;
    boundTransformation.rotate(90);

    helper.bind(boundTransformation);
    QCOMPARE(helper.getOriginalObject(), boundTransformation);
    QCOMPARE(helper.isCacheDirty(), true);
}

void ImageTransformationBaseTest::bindImage() const
{
    QImage image(10, 10, QImage::Format_ARGB32);
    image.setPixel(0, 0, 0xAAFF00);

    ImageTransformationBaseHelper<QImage> helper;
    QCOMPARE(helper.isCacheDirty(), true);

    helper.setCachedObject(image);
    QCOMPARE(helper.getCachedObject(), image);
    QCOMPARE(helper.isCacheDirty(), false);

    QImage boundImage(20, 20, QImage::Format_ARGB32);
    boundImage.setPixel(10, 10, 0xAAFF00);

    helper.bind(boundImage);
    QCOMPARE(helper.getOriginalObject(), boundImage);
    QCOMPARE(helper.isCacheDirty(), true);
}
