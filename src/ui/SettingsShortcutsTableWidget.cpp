/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include "SettingsShortcutsTableWidget.h"

#include "../ui/support/SettingsShortcutsTableWidgetItem.h"
#include <QKeySequenceEdit>

SettingsShortcutsTableWidget::SettingsShortcutsTableWidget(QWidget *parent)
                                        : QTableWidget(parent)
{
}

SettingsShortcutsTableWidget::SettingsShortcutsTableWidget(const int rows, const int columns, QWidget *parent)
                                        : QTableWidget(rows, columns, parent)
{
}

void SettingsShortcutsTableWidget::setItem(const int row, const int column, QTableWidgetItem *item)
{
    QTableWidget::setItem(row, column, item);

    if (item->type() == SettingsShortcutsTableWidgetItem::type)
    {
        if (auto *shortcutItem = dynamic_cast<SettingsShortcutsTableWidgetItem *>(item))
        {
            auto *keySequenceEdit = new QKeySequenceEdit(shortcutItem->keySequence());
            setCellWidget(row, column, keySequenceEdit);
            QObject::connect(keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, shortcutItem, &SettingsShortcutsTableWidgetItem::onKeySequenceChanged);
        }
    }
}

void SettingsShortcutsTableWidget::updateShortcuts() const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        QTableWidgetItem *item = QTableWidget::item(row, 0);
        if (item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            if (auto *shortcutItem = static_cast<SettingsShortcutsTableWidgetItem *>(item))
            {
                if (auto *widget = dynamic_cast<QKeySequenceEdit *>(cellWidget(row, 0)))
                {
                    widget->setKeySequence(shortcutItem->keySequence());
                }
            }
        }
    }
}
