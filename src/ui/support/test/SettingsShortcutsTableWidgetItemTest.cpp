/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "SettingsShortcutsTableWidgetItemTest.h"

#include <QAction>
#include "../SettingsShortcutsTableWidgetItem.h"

void SettingsShortcutsTableWidgetItemTest::onKeySequenceChanged() const
{
    const QKeySequence expectedInitialKeySequence {"i"};
    const QString expectedLabel {"blah"};

    QAction action {expectedLabel};
    action.setShortcut(expectedInitialKeySequence);

    SettingsShortcutsTableWidgetItem item {action};
    QCOMPARE(action.text(), expectedLabel);
    QCOMPARE(action.shortcut(), expectedInitialKeySequence);
    QCOMPARE(item.action().text(), expectedLabel);
    QCOMPARE(item.action().shortcut(), expectedInitialKeySequence);
    QCOMPARE(&item.action(), &action);
    QCOMPARE(item.keySequence(), expectedInitialKeySequence);

    connect(this, SIGNAL(keySequenceChanged(const QKeySequence &)), &item, SLOT(onKeySequenceChanged(const QKeySequence &)));
    const QKeySequence expectedKeySequence {"e"};
    emit(keySequenceChanged(expectedKeySequence));

    QCOMPARE(action.text(), expectedLabel);
    QCOMPARE(action.shortcut(), expectedKeySequence);
    QCOMPARE(item.action().text(), expectedLabel);
    QCOMPARE(item.action().shortcut(), expectedKeySequence);
    QCOMPARE(&item.action(), &action);
    QCOMPARE(item.keySequence(), expectedKeySequence);

    disconnect(this, SIGNAL(keySequenceChanged(const QKeySequence &)), &item, SLOT(onKeySequenceChanged(const QKeySequence &)));
}
