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
    constexpr std::array<enum ByteSize::SizeUnits, 5> sizeUnits {
        ByteSize::SizeUnits::B,
        ByteSize::SizeUnits::kB,
        ByteSize::SizeUnits::MB,
        ByteSize::SizeUnits::GB,
        ByteSize::SizeUnits::TB,
    };

    for (unsigned int i = 0; i < sizeUnits.size(); ++i)
    {
        ByteSize byteSize(1LLU << i * 10);
        const auto [size, unit] = byteSize.humanReadableSize();
        QCOMPARE(size, 1);
        QCOMPARE(unit, sizeUnits[i]);
    }
#if 0
    for (unsigned int i = 0; i < sizeUnits.size(); ++i)
    {
        ByteSize byteSize((1llu << i * 10)-1);
        const auto [size, unit] = byteSize.humanReadableSize();
        qDebug() << static_cast<int>(unit) << size << (1llu << i * 10)-1;
    }
#endif
}
