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

#include "FileSystemSortFilterProxyModel.h"

#include <QFileSystemModel>

FileSystemSortFilterProxyModel::FileSystemSortFilterProxyModel(QObject *parent)
                                        : QSortFilterProxyModel(parent)
{
}

bool FileSystemSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // If sorting by filenames column
    if (sortColumn() == 0)
    {
        auto *fsm = qobject_cast<QFileSystemModel *>(sourceModel());
        const bool asc = sortOrder() == Qt::AscendingOrder;

        QFileInfo leftFileInfo = fsm->fileInfo(left);
        QFileInfo rightFileInfo = fsm->fileInfo(right);

        // Move dirs up
        if (!leftFileInfo.isDir() && rightFileInfo.isDir())
        {
            return !asc;
        }
        if (leftFileInfo.isDir() && !rightFileInfo.isDir())
        {
            return asc;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
