/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <QTransform>

#include "ImageFlipTest.h"
#include "../ImageFlip.h"

void ImageFlipTest::flipHorizontally2x()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    for (int i = 0; i < 2; ++i)
    {
        m_imageFlip.flipHorizontally();
        QCOMPARE(m_imageFlip.isFlippedHorizontally(), i == 0);
        QCOMPARE(m_imageFlip.isFlippedVertically(), false);

        const auto transformAfter = m_imageFlip.transform().value<QTransform>();
        QCOMPARE(transformAfter, transformBefore.rotate(180, Qt::XAxis));
    }
}

void ImageFlipTest::flipVertically2x()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    for (int i = 0; i < 2; ++i)
    {
        m_imageFlip.flipVertically();
        QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
        QCOMPARE(m_imageFlip.isFlippedVertically(), i == 0);

        const auto transformAfter = m_imageFlip.transform().value<QTransform>();
        QCOMPARE(transformAfter, transformBefore.rotate(180, Qt::YAxis));
    }
}

void ImageFlipTest::setFlipHorizontallyTrueFalse()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    for (int i = 0; i < 1; ++i)
    {
        const bool flipped = i == 0;
        m_imageFlip.setFlipHorizontally(flipped);
        QCOMPARE(m_imageFlip.isFlippedHorizontally(), flipped);
        QCOMPARE(m_imageFlip.isFlippedVertically(), false);

        const auto transformAfter = m_imageFlip.transform().value<QTransform>();
        QCOMPARE(flipped, qFuzzyCompare(transformAfter, transformBefore.rotate(180, Qt::XAxis)));
    }
}

void ImageFlipTest::setFlipVerticallyTrueFalse()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    for (int i = 0; i < 1; ++i)
    {
        const bool flipped = i == 0;
        m_imageFlip.setFlipVertically(flipped);
        QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
        QCOMPARE(m_imageFlip.isFlippedVertically(), flipped);

        const auto transformAfter = m_imageFlip.transform().value<QTransform>();
        QCOMPARE(flipped, qFuzzyCompare(transformAfter, transformBefore.rotate(180, Qt::YAxis)));
    }
}

void ImageFlipTest::setFlipHorizontallyFalse()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    m_imageFlip.setFlipHorizontally(false);
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    const auto transformAfter = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(transformAfter, transformBefore);
}

void ImageFlipTest::setFlipVerticallyFalse()
{
    ImageFlip<QTransform> m_imageFlip{};
    auto transformBefore = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    m_imageFlip.setFlipVertically(false);
    QCOMPARE(m_imageFlip.isFlippedHorizontally(), false);
    QCOMPARE(m_imageFlip.isFlippedVertically(), false);

    const auto transformAfter = m_imageFlip.transform().value<QTransform>();
    QCOMPARE(transformAfter, transformBefore);
}

void ImageFlipTest::resetProperties()
{
    ImageFlip<QTransform> m_imageFlip{};
    const auto transformBefore = m_imageFlip.transform().value<QTransform>();

    m_imageFlip.flipHorizontally();
    m_imageFlip.resetProperties();

    const auto transformAfter = m_imageFlip.transform().value<QTransform>();

    QCOMPARE(transformAfter, transformBefore);
}
