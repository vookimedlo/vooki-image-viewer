/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "SettingsShortcutsTableWidgetItem.h"

const int SettingsShortcutsTableWidgetItem::type = QTableWidgetItem::UserType + 1;

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
