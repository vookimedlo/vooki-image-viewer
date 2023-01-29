/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2022  Michal Duda <github@vookimedlo.cz>

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


#include "InfoTableWidget.h"

InfoTableWidget::InfoTableWidget(QWidget *parent) : QTableWidget(parent)
{

}

void InfoTableWidget::displayInformation(const std::vector<std::pair<QString, QString>>& information)
{
    setRowCount(0);

    for (const auto &[label, value] :information)
    {
        const auto row = rowCount();
        insertRow(row);
        setItem(row, 0, new QTableWidgetItem(label));
        setItem(row, 1, new QTableWidgetItem(value));
    }
}
