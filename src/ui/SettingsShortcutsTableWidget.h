#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTableWidget>
#include <QTableWidgetItem>

class SettingsShortcutsTableWidget : public QTableWidget
{
public:
    explicit SettingsShortcutsTableWidget(QWidget *parent = nullptr);
    SettingsShortcutsTableWidget(int rows, int columns, QWidget *parent = nullptr);

    void setItemAtCoordinates(int row, int column, QTableWidgetItem *item);
    void updateShortcuts() const;
};
