/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "SettingsDialogTest.h"
#include <memory>
#include <QKeySequenceEdit>
#include <QSettings>
#include "mock/SettingsDialogMock.h"
#include "../support/test/mock/ui/MainWindow.h"
#include "../../util/testing.h"


void SettingsDialogTest::shortcuts() const
{
    auto defaultSettings = std::make_unique<QSettings>(QSettingsMock::nullFormat, QSettings::UserScope, "test", "test");
    auto userSettings = std::make_unique<QSettings>(QSettingsMock::nullFormat, QSettings::UserScope, "test2", "test2");

    const auto defaultSettingsPointer = defaultSettings.get();
    const auto userSettingsPointer = userSettings.get();

    SettingsDialogMock dialog{ std::move(defaultSettings), std::move(userSettings) };

    MainWindow mainWindow(nullptr);
    const auto allMenus{ mainWindow.allMenus() };

    QCOMPARE(dialog.ui()->tableShortcutsWidget->rowCount(), 0);

    for (const auto menu : allMenus)
        dialog.populateShortcuts(menu);

    // Test that the shortcuts widget contains the same count of entries like the count of menu actions having the shortcut.
    const auto actionsHavingShortcut{ TEST::getAllActionsHavingShortcut(allMenus) };
    QCOMPARE(dialog.ui()->tableShortcutsWidget->rowCount(), actionsHavingShortcut.size());

    // Test that the shortcuts widget contains the same records like the all menu actions having the shortcut.
    ResultingSet actionsHavingShortcutSet;
    std::ranges::for_each(actionsHavingShortcut,
                          [&actionsHavingShortcutSet](const QAction *action) { actionsHavingShortcutSet.emplace(action->toolTip(), action->shortcut()); });

    ResultingSet tableShortcutsWidgetSet;
    const auto tableWidget = dialog.ui()->tableShortcutsWidget;
    for (int i = 0; i < tableWidget->rowCount(); ++i)
        tableShortcutsWidgetSet.emplace(tableWidget->verticalHeaderItem(i)->text(),
                                        dynamic_cast<const QKeySequenceEdit *>(tableWidget->cellWidget(i, 0))->keySequence());

    QCOMPARE(actionsHavingShortcutSet, tableShortcutsWidgetSet);

    // Test the onAccept()
    dialog.onAccept();

    ResultingSet userSettingsOnAccepted;
    std::ranges::for_each(userSettingsPointer->allKeys(), [&userSettingsOnAccepted, userSettingsPointer](const QString &key) {
        if (key.startsWith("viv/shortcut/", Qt::CaseSensitive))
            userSettingsOnAccepted.emplace(key, userSettingsPointer->value(key).value<QKeySequence>());
    });

    ResultingSet userSettingsInitializedFromMenu;
    for (const auto action : TEST::getAllActionsHavingShortcut(allMenus))
        userSettingsInitializedFromMenu.emplace(action->whatsThis(), action->shortcut());

    QCOMPARE(userSettingsOnAccepted, userSettingsInitializedFromMenu);

    // Test the SettingsShortcutsTableWidgetItem::onKeySequenceChanged()
    const auto standardKey = QKeySequence::StandardKey::MoveToNextPage;
    for (int i = 0; i < tableWidget->rowCount(); ++i)
        dynamic_cast<QKeySequenceEdit *>(tableWidget->cellWidget(i, 0))->setKeySequence(standardKey);

    for (const auto action : TEST::getAllActionsHavingShortcut(allMenus))
        QCOMPARE(action->shortcut(), standardKey);

    // Test the onRejected()
    dialog.onRejected();

    ResultingSet userSettingsOnRejected;
    std::ranges::for_each(userSettingsPointer->allKeys(), [&userSettingsOnRejected, userSettingsPointer](const QString &key) {
        if (key.startsWith("viv/shortcut/", Qt::CaseSensitive))
            userSettingsOnRejected.emplace(key, userSettingsPointer->value(key).value<QKeySequence>());
    });

    QCOMPARE(userSettingsOnRejected, userSettingsOnAccepted);

    ResultingSet userSettingsInitializedFromMenuOnRejected;
    for (const auto action : TEST::getAllActionsHavingShortcut(allMenus))
        userSettingsInitializedFromMenuOnRejected.emplace(action->whatsThis(), action->shortcut());

    QCOMPARE(userSettingsInitializedFromMenuOnRejected, userSettingsOnAccepted);


    // Test the onRestoreDefaultsTriggered()
    for (int i = 0; i < tableWidget->rowCount(); ++i)
        dynamic_cast<QKeySequenceEdit *>(tableWidget->cellWidget(i, 0))->setKeySequence(standardKey);

    std::ranges::for_each(userSettingsPointer->allKeys(), [defaultSettingsPointer](const auto &key) {
        static int i = QKeySequence::HelpContents;
        defaultSettingsPointer->setValue(key, QKeySequence(static_cast<QKeySequence::StandardKey>(i++)));
    });

    ResultingSet defaultSettingsShortcuts;
    std::ranges::for_each(defaultSettingsPointer->allKeys(), [&defaultSettingsShortcuts, defaultSettingsPointer](const QString &key) {
        if (key.startsWith("viv/shortcut/", Qt::CaseSensitive))
            defaultSettingsShortcuts.emplace(key, defaultSettingsPointer->value(key).value<QKeySequence>());
    });

    dialog.onRestoreDefaultsTriggered();

    ResultingSet userSettingsOnRestoreDefaults;
    std::ranges::for_each(userSettingsPointer->allKeys(), [&userSettingsOnRestoreDefaults, userSettingsPointer](const QString &key) {
        if (key.startsWith("viv/shortcut/", Qt::CaseSensitive))
            userSettingsOnRestoreDefaults.emplace(key, userSettingsPointer->value(key).value<QKeySequence>());
    });

    QCOMPARE(userSettingsOnRestoreDefaults, userSettingsOnAccepted);

    ResultingSet userSettingsInitializedFromMenuOnRestoreDefaults;
    for (const auto action : TEST::getAllActionsHavingShortcut(allMenus))
        userSettingsInitializedFromMenuOnRestoreDefaults.emplace(action->whatsThis(), action->shortcut());

    QCOMPARE(userSettingsInitializedFromMenuOnRestoreDefaults, defaultSettingsShortcuts);
}
