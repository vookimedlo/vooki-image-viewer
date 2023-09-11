/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "MiscTest.h"
#include "../misc.h"

void MiscTest::convertFormatsToFilters() const
{
    #define prefix "*."

    #define one "ext_one"
    #define two "ext_two"
    #define three "ext_three"

    const QList<QByteArray> input {one, two, three};
    const QStringList expectedList {prefix one, prefix two, prefix three};
    const QStringList resultingList {Util::convertFormatsToFilters(input)};
    QCOMPARE(resultingList, expectedList);
}
