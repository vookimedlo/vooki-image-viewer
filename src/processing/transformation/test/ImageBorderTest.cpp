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

void ImageBorderTest::checkAllPixels(const QImage &image, const QRgb color, const QPoint& origin, const QPoint& originMax) const
{
    const auto& [x, y] = origin;
    const auto& [xMax, yMax] = originMax;
    for (int i = x; i < xMax; ++i)
        for (int j = y; j < yMax; ++j)
            QCOMPARE(image.pixel(i, j), color);
}

void ImageBorderTest::checkBorder(const QImage &image, const QRgb borderColor, const int borderSize, const BorderPosition position, const QPoint& origin, const QPoint& originMax) const
{
    const auto& [x, y] = origin;
    const auto& [xMax, yMax] = originMax;
    switch (position)
    {
        using enum BorderPosition;
        case TOP:
            checkAllPixels(image, borderColor, {x, y}, {xMax - borderSize, y + borderSize});
            break;
        case BOTTOM:
            checkAllPixels(image, borderColor, {x, yMax - borderSize}, {xMax - borderSize, yMax});
            break;
        case LEFT:
            checkAllPixels(image, borderColor, {x, y}, {x + borderSize, yMax});
            break;
        case RIGHT:
            checkAllPixels(image, borderColor, {xMax - borderSize, y}, {xMax, yMax});
            break;
    }
}


void ImageBorderTest::checkBorder(const QImage &image, const QRgb borderColor, const int borderSize, const BorderPosition position) const
{
    checkBorder(image, borderColor, borderSize, position, {0, 0}, {image.width(), image.height()});
}

void ImageBorderTest::checkTransformationWithOffset(int offsetX, int offsetY) const
{
    constexpr auto fillingColor = Qt::blue;

    // Area size is set, border is drawn, test the input image is the same as the output one with respect to borders and area on all sides while offset is changed.

    QImage image(50, 120, QImage::Format_RGB32);
    image.fill(fillingColor);

    const auto areaBorderWidth = offsetX;
    const auto areaBorderHeight = offsetY;

    ImageBorder<QImage> imageBorder;
    imageBorder.bind(image);
    imageBorder.setDrawBorder(true);
    imageBorder.setAreaSize({ 20, 24 });
    imageBorder.setImageOffsetX(areaBorderWidth);
    imageBorder.setImageOffsetY(areaBorderHeight);
    QCOMPARE(imageBorder.isCacheDirty(), true);
    QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
    QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);

    const QImage outputImage = imageBorder.transform().value<QImage>();

    for (const auto position : { BorderPosition::TOP, BorderPosition::LEFT })
    {
        const auto borderWidth = [&position, &areaBorderWidth, &areaBorderHeight] {
            switch (position)
            {
                using enum BorderPosition;
                case TOP:
                    return (ImageBorder<QImage>::borderWidth - areaBorderHeight) > 0 ? ImageBorder<QImage>::borderWidth - areaBorderHeight : 1;
                case LEFT:
                    return (ImageBorder<QImage>::borderWidth - areaBorderWidth) > 0 ? ImageBorder<QImage>::borderWidth - areaBorderWidth : 1;
                case BOTTOM:
                    [[fallthrough]];
                case RIGHT:
                    return ImageBorder<QImage>::borderWidth;
            }
        }();

        checkBorder(outputImage,
                    imageBorder.getBorderColor().rgba(),
                    borderWidth,
                    position,
                    {0, 0},
                    {outputImage.width() - areaBorderWidth, outputImage.height() - areaBorderHeight});
    }

    for (const auto position : { BorderPosition::BOTTOM, BorderPosition::RIGHT })
    {
        checkBorder(outputImage,
                    imageBorder.getBorderColor().rgba(),
                    ImageBorder<QImage>::borderWidth,
                    position,
                    {areaBorderWidth, areaBorderHeight},
                    {outputImage.width() - areaBorderWidth, outputImage.height() - areaBorderHeight});
    }

    checkAllPixels(outputImage,
                   QColor(fillingColor).rgba(),
                   {(ImageBorder<QImage>::borderWidth - areaBorderWidth) > 0 ? ImageBorder<QImage>::borderWidth - areaBorderWidth : 1,
                   (ImageBorder<QImage>::borderWidth - areaBorderHeight) > 0 ? ImageBorder<QImage>::borderWidth - areaBorderHeight : 1},
                   {outputImage.width() - areaBorderWidth - ImageBorder<QImage>::borderWidth,
                   outputImage.height() - areaBorderHeight - ImageBorder<QImage>::borderWidth});
}


void ImageBorderTest::transform() const
{
    constexpr auto fillingColor = Qt::blue;

    {
        // Area size is not set, border is not drawn, test the input image is the same as the output one.

        QImage image(10, 10, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);
        QCOMPARE(imageBorder.transform().value<QImage>(), image);
    }

    {
        // Area size is not set, border is not drawn, test the input image is the same as the output one. Cache is not dirty after transform.

        QImage image(10, 10, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);
        QCOMPARE(imageBorder.transform().value<QImage>(), image);
        QCOMPARE(imageBorder.isCacheDirty(), false);
        QCOMPARE(imageBorder.transform().value<QImage>(), image);
        QCOMPARE(imageBorder.isCacheDirty(), false);
    }

    {
        // Area size is not set, border is drawn, test the input image is the same as the output one with respect to borders on all sides.

        QImage image(12, 12, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        imageBorder.setDrawBorder(true);
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);

        const QImage outputImage = imageBorder.transform().value<QImage>();
        for (const auto position : { BorderPosition::TOP, BorderPosition::BOTTOM, BorderPosition::LEFT, BorderPosition::RIGHT })
            checkBorder(outputImage, imageBorder.getBorderColor().rgba(), ImageBorder<QImage>::borderWidth, position);

        checkAllPixels(outputImage,
                       QColor(fillingColor).rgba(),
                       {ImageBorder<QImage>::borderWidth, ImageBorder<QImage>::borderWidth},
                       {outputImage.width() - ImageBorder<QImage>::borderWidth, outputImage.height() - ImageBorder<QImage>::borderWidth});
    }

    {
        // Area size is set (equal to the input image), border is drawn, test the input image is the same as the output one with respect to borders on all sides.

        QImage image(12, 12, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        imageBorder.setDrawBorder(true);
        imageBorder.setAreaSize({ 12, 12 });
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);

        const QImage outputImage = imageBorder.transform().value<QImage>();
        for (const auto position : { BorderPosition::TOP, BorderPosition::BOTTOM, BorderPosition::LEFT, BorderPosition::RIGHT })
            checkBorder(outputImage, imageBorder.getBorderColor().rgba(), ImageBorder<QImage>::borderWidth, position);

        checkAllPixels(outputImage,
                       QColor(fillingColor).rgba(),
                       {ImageBorder<QImage>::borderWidth, ImageBorder<QImage>::borderWidth},
                       {outputImage.width() - ImageBorder<QImage>::borderWidth, outputImage.height() - ImageBorder<QImage>::borderWidth});
    }

    {
        // Area size is set (the input image size multiplied by 2), border is drawn, test the input image is the same as the output one with respect to borders and area on all sides.

        QImage image(12, 12, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        imageBorder.setDrawBorder(true);
        imageBorder.setAreaSize({ 24, 24 });
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);

        const QImage outputImage = imageBorder.transform().value<QImage>();
        const auto areaBorderWidth = (imageBorder.getAreaSize().width() - image.width()) / 2;
        const auto areaBorderHeight = (imageBorder.getAreaSize().height() - image.height()) / 2;

        for (const auto position : { BorderPosition::TOP, BorderPosition::BOTTOM, BorderPosition::LEFT, BorderPosition::RIGHT })
        {
            checkBorder(outputImage,
                        imageBorder.getBorderColor().rgba(),
                        ImageBorder<QImage>::borderWidth,
                        position,
                        {areaBorderWidth, areaBorderHeight},
                        {outputImage.width() - areaBorderWidth, outputImage.height() - areaBorderHeight});
        }

        checkAllPixels(outputImage,
                       QColor(fillingColor).rgba(),
                       {areaBorderWidth + ImageBorder<QImage>::borderWidth, areaBorderHeight + ImageBorder<QImage>::borderWidth},
                       {outputImage.width() - areaBorderWidth - ImageBorder<QImage>::borderWidth, outputImage.height() - areaBorderHeight - ImageBorder<QImage>::borderWidth});
    }

    {
        // Area size is not set, border is not drawn, test the input image is the same as the output one.
        // Negative offsets are zeroed during transformation.

        QImage image(10, 10, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        imageBorder.setImageOffsetX(-20);
        imageBorder.setImageOffsetY(-10);
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);
        QCOMPARE(imageBorder.transform().value<QImage>(), image);
        QCOMPARE(imageBorder.getImageOffsetX(), 0);
        QCOMPARE(imageBorder.getImageOffsetY(), 0);
    }

    {
        // Area size is set (image size - offsets are smaller than the area size), border is drawn, test the input image is the same as the output one.
        // Offsets are zeroed during transformation.

        QImage image(10, 10, QImage::Format_RGB32);
        image.fill(fillingColor);

        ImageBorder<QImage> imageBorder;
        imageBorder.bind(image);
        imageBorder.setAreaSize({ 80, 60 });
        imageBorder.setImageOffsetX(200);
        imageBorder.setImageOffsetY(100);
        imageBorder.setDrawBorder(true);
        QCOMPARE(imageBorder.isCacheDirty(), true);
        QCOMPARE_NE(imageBorder.getBorderColor(), fillingColor);
        QCOMPARE_NE(imageBorder.getBackgroundColor(), fillingColor);
        const QImage outputImage = imageBorder.transform().value<QImage>();
        QCOMPARE(imageBorder.getImageOffsetX(), 0);
        QCOMPARE(imageBorder.getImageOffsetY(), 0);

        for (const auto position : { BorderPosition::TOP, BorderPosition::LEFT,  BorderPosition::BOTTOM, BorderPosition::RIGHT })
            checkBorder(outputImage,
                        imageBorder.getBorderColor().rgba(),
                        ImageBorder<QImage>::borderWidth,
                        position,
                        {(outputImage.width() - image.width()) / 2, (outputImage.height() - image.height()) / 2},
                        {outputImage.width() - (outputImage.width() - image.width()) / 2, outputImage.height() - (outputImage.height() - image.height()) / 2});
    }

    {
        for (int x = 1; x < 10; ++x)
            checkTransformationWithOffset(x, 1);
    }

    {
        for (int y = 1; y < 10; ++y)
            checkTransformationWithOffset(2, y);
    }

    {
        for (int x = 1; x < 10; ++x)
            for (int y = 1; y < 10; ++y)
                checkTransformationWithOffset(x, y);
    }
}
