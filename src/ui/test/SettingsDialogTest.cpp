/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "SettingsDialogTest.h"
#include <memory>
#include <unordered_set>
#include <QKeySequenceEdit>
#include <QSettings>
#include "mock/SettingsDialogMock.h"
#include "../support/test/mock/ui/MainWindow.h"
#include "../../util/testing.h"


void SettingsDialogTest::populateShortcuts() const
{
    auto defaultSettings = std::make_unique<QSettings>(QSettingsMock::nullFormat, QSettings::UserScope, "test", "test");
    auto userSettings = std::make_unique<QSettings>(QSettingsMock::nullFormat, QSettings::UserScope, "test2", "test2");

    SettingsDialogMock dialog {std::move(defaultSettings), std::move(userSettings)};

    MainWindow mainWindow(nullptr);
    const auto allMenus {mainWindow.allMenus()};

    QCOMPARE(dialog.dialog()->tableShortcutsWidget->rowCount(), 0);

    for (const auto menu : allMenus)
        dialog.populateShortcuts(menu);

    const auto actionsHavingShortcut {TEST::getAllActionsHavingShortcut(allMenus)};
    QCOMPARE(dialog.dialog()->tableShortcutsWidget->rowCount(), actionsHavingShortcut.size());

    struct hashFunction
    {
        size_t operator()(const std::pair<QString, QKeySequence> &x) const
        {
            return qHash((x.first + x.second.toString()), 1234);
        }
    };
    using ResultingSet = std::unordered_set<std::pair<QString, QKeySequence>, hashFunction>;

    ResultingSet actionsHavingShortcutSet;
    std::ranges::for_each(actionsHavingShortcut, [&actionsHavingShortcutSet](const QAction* action) {
        actionsHavingShortcutSet.emplace(action->toolTip(), action->shortcut());
    });

    ResultingSet tableShortcutsWidgetSet;
    const auto tableWidget = dialog.dialog()->tableShortcutsWidget;
    for (int i = 0; i < tableWidget->rowCount(); ++i)
        tableShortcutsWidgetSet.emplace(tableWidget->verticalHeaderItem(i)->text(), dynamic_cast<const QKeySequenceEdit *>(tableWidget->cellWidget(i,0))->keySequence());

    QCOMPARE(actionsHavingShortcutSet, tableShortcutsWidgetSet);
}
