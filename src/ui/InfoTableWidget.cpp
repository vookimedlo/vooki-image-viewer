/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "InfoTableWidget.h"

InfoTableWidget::InfoTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void InfoTableWidget::displayInformation(const std::vector<std::pair<QString, QString>>& information)
{
    setRowCount(0);

    for (const auto &[label, value] :information)
    {
        const auto row { rowCount() };
        insertRow(row);
        setItem(row, 0, new QTableWidgetItem(label));
        setItem(row, 1, new QTableWidgetItem(value));
    }
}
