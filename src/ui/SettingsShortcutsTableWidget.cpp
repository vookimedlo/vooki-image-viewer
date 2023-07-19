/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "SettingsShortcutsTableWidget.h"

#include "../ui/support/SettingsShortcutsTableWidgetItem.h"
#include <memory>
#include <QKeySequenceEdit>

SettingsShortcutsTableWidget::SettingsShortcutsTableWidget(QWidget *parent)
                                        : QTableWidget(parent)
{
}

SettingsShortcutsTableWidget::SettingsShortcutsTableWidget(const int rows, const int columns, QWidget *parent)
                                        : QTableWidget(rows, columns, parent)
{
}

void SettingsShortcutsTableWidget::setItemAtCoordinates(int row, int column, QTableWidgetItem *item)
{
    QTableWidget::setItem(row, column, item);

    if (item->type() == SettingsShortcutsTableWidgetItem::type)
    {
        if (const auto * const shortcutItem = dynamic_cast<SettingsShortcutsTableWidgetItem *>(item))
        {
            auto keySequenceEdit = std::make_unique<QKeySequenceEdit>(shortcutItem->keySequence());
            QObject::connect(keySequenceEdit.get(), &QKeySequenceEdit::keySequenceChanged, shortcutItem, &SettingsShortcutsTableWidgetItem::onKeySequenceChanged);
            setCellWidget(row, column, keySequenceEdit.release()); // ownership passed
        }
    }
}

void SettingsShortcutsTableWidget::updateShortcuts() const
{
    for (int row = 0; row < rowCount(); ++row)
    {
        if (const auto * const item = QTableWidget::item(row, 0); item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            if (const auto * const shortcutItem = dynamic_cast<const SettingsShortcutsTableWidgetItem *>(item))
            {
                if (auto * const widget = dynamic_cast<QKeySequenceEdit *>(cellWidget(row, 0)))
                {
                    widget->setKeySequence(shortcutItem->keySequence());
                }
            }
        }
    }
}
