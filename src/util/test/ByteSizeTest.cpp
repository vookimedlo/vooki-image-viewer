/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <array>
#include <cmath>

#include "ByteSizeTest.h"
#include "../ByteSize.h"

using enum ByteSize::SizeUnits;

void ByteSizeTest::humanReadableSizeForValue1() const
{
    constexpr int arraySize = 5;
    constexpr std::array sizeUnits{
        B,
        kB,
        MB,
        GB,
        TB,
    };

    constexpr std::array<const char *const, arraySize> units{
        "B",
        "kB",
        "MB",
        "GB",
        "TB",
    };

    static_assert(units.size() == sizeUnits.size());

    for (unsigned int i = 0; i < arraySize; ++i)
    {
        ByteSize byteSize(1LLU << i * 10);
        const auto [size, unit] = byteSize.humanReadableSize();
        QCOMPARE(size, 1);
        QCOMPARE(unit, sizeUnits[i]);
        QCOMPARE(byteSize.getUnit(unit), units[i]);
    }
}

void ByteSizeTest::humanReadableSizeForEdgeValueMinus1() const
{
    constexpr int arraySize = 5;
    constexpr std::array expectedUnits_minus1{
        B,
        B,
        kB,
        MB,
        GB,
    };

    constexpr std::array<double, arraySize> expectedSizes_minus1{
        0,
        1023,
        1024,
        1024,
        1024,
    };

    static_assert(expectedUnits_minus1.size() == expectedSizes_minus1.size());

    for (unsigned int i = 0; i < arraySize; ++i)
    {
        ByteSize byteSize((1LLU << i * 10) - 1);
        const auto [size, unit] = byteSize.humanReadableSize();
        QCOMPARE(size, expectedSizes_minus1[i]);
        QCOMPARE(unit, expectedUnits_minus1[i]);
    }
}

void ByteSizeTest::humanReadableSizeForEdgeValuePlus1() const
{
    constexpr int arraySize = 5;
    constexpr std::array expectedUnits_plus1{
        B,
        kB,
        MB,
        GB,
        TB,
    };

    constexpr std::array<double, arraySize> expectedSizes_plus1{
        2,
        1.1,
        1.1,
        1.1,
        1.1,
    };

    static_assert(expectedUnits_plus1.size() == expectedSizes_plus1.size());

    for (unsigned int i = 0; i < arraySize; ++i)
    {
        ByteSize byteSize((1LLU << i * 10) + 1);
        const auto [size, unit] = byteSize.humanReadableSize();
        QCOMPARE(size, expectedSizes_plus1[i]);
        QCOMPARE(unit, expectedUnits_plus1[i]);
    }
}
