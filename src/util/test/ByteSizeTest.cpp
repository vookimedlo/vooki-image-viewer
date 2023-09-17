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

void ByteSizeTest::humanReadableSize() const
{
    constexpr int arraySize = 5;

    {
        constexpr std::array<enum ByteSize::SizeUnits, arraySize> sizeUnits{
            ByteSize::SizeUnits::B,
            ByteSize::SizeUnits::kB,
            ByteSize::SizeUnits::MB,
            ByteSize::SizeUnits::GB,
            ByteSize::SizeUnits::TB,
        };

        constexpr std::array<const char *const, arraySize> units{
            "B",
            "kB",
            "MB",
            "GB",
            "TB",
        };

        for (unsigned int i = 0; i < arraySize; ++i)
        {
            ByteSize byteSize(1LLU << i * 10);
            const auto [size, unit] = byteSize.humanReadableSize();
            QCOMPARE(size, 1);
            QCOMPARE(unit, sizeUnits[i]);
            QCOMPARE(byteSize.getUnit(unit), units[i]);
        }
    }

    {
        constexpr std::array<enum ByteSize::SizeUnits, arraySize> expectedUnits_minus1{
            ByteSize::SizeUnits::B,
            ByteSize::SizeUnits::B,
            ByteSize::SizeUnits::kB,
            ByteSize::SizeUnits::MB,
            ByteSize::SizeUnits::GB,
        };

        constexpr std::array<double, arraySize> expectedSizes_minus1{
            0,
            1023,
            1024,
            1024,
            1024,
        };

        for (unsigned int i = 0; i < arraySize; ++i)
        {
            ByteSize byteSize((1LLU << i * 10) - 1);
            const auto [size, unit] = byteSize.humanReadableSize();
            QCOMPARE(size, expectedSizes_minus1[i]);
            QCOMPARE(unit, expectedUnits_minus1[i]);
        }
    }

    {
        constexpr std::array<enum ByteSize::SizeUnits, arraySize> expectedUnits_plus1{
            ByteSize::SizeUnits::B,
            ByteSize::SizeUnits::kB,
            ByteSize::SizeUnits::MB,
            ByteSize::SizeUnits::GB,
            ByteSize::SizeUnits::TB,
        };

        constexpr std::array<double, arraySize> expectedSizes_plus1{
            2,
            1.1,
            1.1,
            1.1,
            1.1,
        };

        for (unsigned int i = 0; i < arraySize; ++i)
        {
            ByteSize byteSize((1LLU << i * 10) + 1);
            const auto [size, unit] = byteSize.humanReadableSize();
            QCOMPARE(size, expectedSizes_plus1[i]);
            QCOMPARE(unit, expectedUnits_plus1[i]);
        }
    }
}
