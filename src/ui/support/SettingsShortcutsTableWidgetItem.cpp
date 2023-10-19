/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "SettingsShortcutsTableWidgetItem.h"

SettingsShortcutsTableWidgetItem::SettingsShortcutsTableWidgetItem(QAction &action)
                                        : QObject()
                                        , QTableWidgetItem(SettingsShortcutsTableWidgetItem::type)
                                        , m_action(action)
                                        , m_keySequence(action.shortcut())
{
}

QAction &SettingsShortcutsTableWidgetItem::action() const
{
    return m_action;
}

QKeySequence SettingsShortcutsTableWidgetItem::keySequence() const
{
    return m_keySequence;
}

void SettingsShortcutsTableWidgetItem::onKeySequenceChanged(const QKeySequence &keySequence)
{
    m_keySequence = keySequence;
    m_action.setShortcut(keySequence);
}
