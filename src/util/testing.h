#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <QMenu>
#include <queue>
#include "misc.h"

namespace TEST
{
    template<typename TestClass>
    void runTests(int argc, char *argv[], int *status)
    {
        Q_ASSERT(status);

        ::QTest::Internal::callInitMain<TestClass>();
        QApplication app(argc, argv);
        QApplication::setAttribute(Qt::AA_Use96Dpi, true);
        QTEST_DISABLE_KEYPAD_NAVIGATION TestClass tc;
        QTEST_SET_MAIN_SOURCE_PATH
        *status |= QTest::qExec(&tc, argc, argv);
    }

    template<typename T, size_t N>
    std::vector<const QAction *> getAllActionsHavingShortcut(const std::array<T, N> &menus)
    {
        std::vector<const QAction *> result;
        std::ranges::for_each(menus, [&result](const QMenu *const menu) {
            const auto shortcuts = Util::getAllActionsHavingShortcut(menu);
            result.insert(result.end(), shortcuts.cbegin(), shortcuts.cend());
        });

        return result;
    }
}

#if not defined QCOMPARE_NE
    #define QCOMPARE_NE(lhs, rhs) QVERIFY(lhs != rhs)
#endif
