/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ByteSize.h"
#include <cfenv>
#include <cmath>

ByteSize::ByteSize(uint64_t size) : m_size(size)
{
}

std::pair<double, enum ByteSize::SizeUnits> ByteSize::humanReadableSize() const {
    int i{};
    auto mantissa = static_cast<long double>(m_size);
    std::fesetround(FE_DOWNWARD);
    for (; std::llrint(mantissa) >= 1024; mantissa /= 1024., ++i);
    mantissa = std::ceil(mantissa * 10.) / 10.;
    return {mantissa, SizeUnits{i}};
}

QString ByteSize::getUnit(enum ByteSize::SizeUnits unit) const {
    return units[unit];
}
