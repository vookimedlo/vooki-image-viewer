/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTransform>

#include "ImageRotationTest.h"
#include "../ImageRotation.h"

void ImageRotationTest::rotateLeft4x() const
{
    ImageRotation<QTransform> m_imageRotation {};
    auto transformBefore =  m_imageRotation.transform().value<QTransform>();

    for (int i = 0; i < 4; ++i)
    {
        m_imageRotation.rotateLeft();

        const auto transformAfter =  m_imageRotation.transform().value<QTransform>();
        QCOMPARE(transformAfter, transformBefore.rotate(-90));
    }
}

void ImageRotationTest::rotateRight4x() const
{
    ImageRotation<QTransform> m_imageRotation {};
    auto transformBefore =  m_imageRotation.transform().value<QTransform>();

    for (int i = 0; i < 4; ++i)
    {
        m_imageRotation.rotateRight();

        const auto transformAfter =  m_imageRotation.transform().value<QTransform>();
        QCOMPARE(transformAfter, transformBefore.rotate(90));
    }
}

void ImageRotationTest::rotateLeftRight() const
{
    ImageRotation<QTransform> m_imageRotation {};
    const auto transformBefore =  m_imageRotation.transform().value<QTransform>();

    m_imageRotation.rotateLeft();
    m_imageRotation.rotateRight();

    const auto transformAfter =  m_imageRotation.transform().value<QTransform>();

    QCOMPARE(transformAfter.isRotating(), false);
    QCOMPARE(transformAfter, transformBefore);
}

void ImageRotationTest::rotateRightLeft() const
{
    ImageRotation<QTransform> m_imageRotation {};
    const auto transformBefore =  m_imageRotation.transform().value<QTransform>();

    m_imageRotation.rotateRight();
    m_imageRotation.rotateLeft();

    const auto transformAfter =  m_imageRotation.transform().value<QTransform>();

    QCOMPARE(transformAfter.isRotating(), false);
    QCOMPARE(transformAfter, transformBefore);
}

void ImageRotationTest::transform2x() const
{
    ImageRotation<QTransform> m_imageRotation {};
    auto transformBefore =  m_imageRotation.transform().value<QTransform>();
    QCOMPARE(m_imageRotation.transform().value<QTransform>(), transformBefore);

    for (int i = 0; i < 4; ++i)
    {
        m_imageRotation.rotateRight();

        const auto transformAfter =  m_imageRotation.transform().value<QTransform>();
        QCOMPARE(m_imageRotation.transform().value<QTransform>(), transformAfter);
        QCOMPARE(transformAfter, transformBefore.rotate(90));
    }
}

void ImageRotationTest::resetProperties() const
{
    ImageRotation<QTransform> m_imageRotation {};
    const auto transformBefore =  m_imageRotation.transform().value<QTransform>();

    m_imageRotation.rotateRight();
    m_imageRotation.rotateRight();
    m_imageRotation.resetProperties();

    const auto transformAfter =  m_imageRotation.transform().value<QTransform>();

    QCOMPARE(transformAfter.isRotating(), false);
    QCOMPARE(transformAfter, transformBefore);
}
