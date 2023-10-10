/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTransform>

#include "ImageLoaderTest.h"
#include "../ImageLoader.h"

QString ImageLoaderTest::makeAbsolutePath(const QString &file) const
{
    return QDir::cleanPath(m_absolutePath + QDir::separator() + file);
}

void ImageLoaderTest::open() const
{
    ImageLoader loader;
    QCOMPARE(loader.loadImage("@#$%"), false);
    QCOMPARE(loader.loadImage(makeAbsolutePath(ImageLoaderTest::png1FilePath)), true);
}

void ImageLoaderTest::getImageNotAnimated() const
{
    ImageLoader loader;
    QCOMPARE(loader.loadImage(makeAbsolutePath(ImageLoaderTest::png1FilePath)), true);

    QImageReader reader(makeAbsolutePath(ImageLoaderTest::png1FilePath));
    const QImage expectedImage(reader.read());
    QCOMPARE(loader.getImage(), expectedImage);
    QCOMPARE(loader.getNextImage(), expectedImage);
    QCOMPARE(loader.isAnimated(), false);
    // If the format does not support animation, 0 is returned.
    QCOMPARE(loader.imageCount(), 0);
    QCOMPARE(loader.nextImageDelay(), 0);
}

void ImageLoaderTest::getImageAnimated() const
{
    ImageLoader loader;
    QCOMPARE(loader.loadImage(makeAbsolutePath(ImageLoaderTest::animatedNumbersFilePath)), true);

    constexpr auto expectedDelay = 200;
    QImageReader reader(makeAbsolutePath(ImageLoaderTest::animatedNumbersFilePath));
    const QImage expectedImage1(reader.read());
    QCOMPARE(loader.getImage(), expectedImage1);
    QCOMPARE(loader.isAnimated(), true);
    QCOMPARE(loader.imageCount(), 3);
    QCOMPARE(loader.nextImageDelay(), expectedDelay);
    QCOMPARE(loader.getNextImage(), reader.read());
    QCOMPARE(loader.nextImageDelay(), expectedDelay);
    QCOMPARE(loader.getNextImage(), reader.read());
    QCOMPARE(loader.nextImageDelay(), expectedDelay);
    QCOMPARE(loader.getNextImage(), expectedImage1);
}
