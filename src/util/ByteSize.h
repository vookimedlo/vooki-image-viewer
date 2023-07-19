#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QCoreApplication>
#include <cstdint>
#include <utility>
#include "EnumClassArray.h"

class ByteSize
{
public:
    enum class SizeUnits {
        B,
        kB,
        MB,
        GB,
        TB,
    };

    explicit ByteSize(uint64_t size);

    [[nodiscard]] std::pair<double, enum ByteSize::SizeUnits> humanReadableSize() const;
    [[nodiscard]] QString getUnit(enum ByteSize::SizeUnits unit) const;

private:
    EnumClassArray<QString, 5> units {
        QCoreApplication::translate("File size - Units: byte", "B"),
        QCoreApplication::translate("File size - Units: kilobyte", "kB"),
        QCoreApplication::translate("File size - Units: megabyte", "MB"),
        QCoreApplication::translate("File size - Units: gigabyte", "GB"),
        QCoreApplication::translate("File size - Units: terabyte", "TB"),
    };

    uint64_t m_size;
};
