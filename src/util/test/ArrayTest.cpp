/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ArrayTest.h"
#include "../array.h"

void ArrayTest::constructArray() const
{
    QString testingObject1;
    QString testingObject2;
    QString testingObject3;
    QString testingObject4;

    const auto firstArray { std::to_array({ testingObject1, testingObject2 }) };
    const auto secondArray { std::to_array({ testingObject3, testingObject4 }) };
    const std::array<QString, firstArray.size() + secondArray.size()> joinedArray { Array::concatenate<QString>(firstArray, secondArray) };

    QCOMPARE(joinedArray[0], firstArray[0]);
    QCOMPARE(joinedArray[1], firstArray[1]);
    QCOMPARE(joinedArray[2], secondArray[0]);
    QCOMPARE(joinedArray[3], secondArray[1]);
}

void ArrayTest::constructPointerArray() const
{
    ArrayTest testingObject1;
    ArrayTest testingObject2;
    ArrayTest testingObject3;
    ArrayTest testingObject4;

    const std::array<const ArrayTest* const, 2> firstArray { &testingObject1, &testingObject2 };
    const std::array<const ArrayTest* const, 2> secondArray { &testingObject3, &testingObject4 };
    const std::array<const QObject* const, firstArray.size() + secondArray.size()> joinedArray { Array::concatenate<const QObject* const>(firstArray, secondArray) };

    QCOMPARE(joinedArray[0], firstArray[0]);
    QCOMPARE(joinedArray[1], firstArray[1]);
    QCOMPARE(joinedArray[2], secondArray[0]);
    QCOMPARE(joinedArray[3], secondArray[1]);
}
