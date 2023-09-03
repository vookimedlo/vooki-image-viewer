/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTransform>

#include "RotatingIndexTest.h"
#include "../RotatingIndex.h"

void RotatingIndexTest::postDecrement4x()
{
    constexpr int maxValuePlus1 = 4;
    constexpr int initialValue = 0;

    RotatingIndex<int> index(initialValue, maxValuePlus1);
    const int indexBefore = index;
    QCOMPARE(indexBefore, initialValue);

    // overflow - test the maxValuePlus1 - 1
    const int indexAfter = index--;
    QCOMPARE(indexAfter, initialValue);
    QCOMPARE(index, maxValuePlus1 - 1);

    for (int i = maxValuePlus1 - 1; initialValue < i; --i )
    {
        const int indexAfter = index--;
        QCOMPARE(indexAfter, i);
        QCOMPARE(index, i - 1);
    }
}

void RotatingIndexTest::preDecrement4x()
{
    constexpr int maxValuePlus1 = 4;
    constexpr int initialValue = 0;

    RotatingIndex<int> index(initialValue, maxValuePlus1);
    const int indexBefore = index;
    QCOMPARE(indexBefore, initialValue);

    // overflow - test the maxValuePlus1 - 1
    const int indexAfter = --index;
    QCOMPARE(indexAfter, maxValuePlus1 - 1);
    QCOMPARE(index, maxValuePlus1 - 1);

    for (int i = maxValuePlus1 - 1; initialValue < i; --i )
    {
        const int indexAfter = --index;
        QCOMPARE(indexAfter, i - 1);
        QCOMPARE(index, i - 1);
    }
}

void RotatingIndexTest::postIncrement4x()
{
    constexpr int initialValue = 0;
    constexpr int maxValuePlus1 = 4;
    RotatingIndex<int> index(initialValue, maxValuePlus1);
    const int indexBefore = index;
    QCOMPARE(indexBefore, initialValue);

    for (int i = initialValue; i < maxValuePlus1 - 1; ++i )
    {
        const int indexAfter = index++;
        QCOMPARE(indexAfter, i);
        QCOMPARE(index, i + 1);
    }

    // overflow - test the initial value
    const int indexAfter = index++;
    QCOMPARE(indexAfter, maxValuePlus1 - 1);
    QCOMPARE(index, initialValue);
}

void RotatingIndexTest::preIncrement4x()
{
    constexpr int initialValue = 0;
    constexpr int maxValuePlus1 = 4;
    RotatingIndex<int> index(initialValue, maxValuePlus1);
    const int indexBefore = index;
    QCOMPARE(indexBefore, initialValue);

    for (int i = initialValue; i < maxValuePlus1 - 1; ++i )
    {
        const int indexAfter = ++index;
        QCOMPARE(indexAfter, i + 1);
        QCOMPARE(index, i + 1);
    }

    // overflow - test the initial value
    const int indexAfter = ++index;
    QCOMPARE(indexAfter, initialValue);
    QCOMPARE(index, initialValue);
}
