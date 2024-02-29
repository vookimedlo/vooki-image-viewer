/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
        const auto *fsm = qobject_cast<QFileSystemModel *>(sourceModel());
        const bool asc = sortOrder() == Qt::AscendingOrder;

        const QFileInfo leftFileInfo = fsm->fileInfo(left);
        const QFileInfo rightFileInfo = fsm->fileInfo(right);

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
