/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "EnumClassArrayTest.h"
#include "../EnumClassArray.h"

void EnumClassArrayTest::constructionAndRetrival() const
{
    enum class Index
    {
        one,
        two,
        three,
    };

    constexpr const char * const one_p = "one";
    constexpr const char * const two_p = "two";
    constexpr const char * const three_p = "three";

    EnumClassArray<QString, 3u> units {
        one_p,
        two_p,
        three_p,
    };

    QString one = units[Index::one];
    QCOMPARE(one_p, one);
    const QString &two = units[Index::two];
    QCOMPARE(two_p, two);
}
