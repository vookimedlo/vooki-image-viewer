/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "MiscTest.h"
#include <string>
#include "../misc.h"

void MiscTest::convertFormatsToFilters() const
{
    const std::string prefix("*.");

    const std::string one("ext_one");
    const std::string two("ext_two");
    const std::string three("ext_three");

    const QList<QByteArray> input {one.c_str(), two.c_str(), three.c_str()};
    const QStringList expectedList {(prefix + one).c_str(), (prefix + two).c_str(), (prefix + three).c_str()};
    const QStringList resultingList {Util::convertFormatsToFilters(input)};
    QCOMPARE(resultingList, expectedList);
}
