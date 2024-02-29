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

bool FileSystemSortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // If sorting by filenames column
    if (sortColumn() == 0)
    {
        const auto *fsm = qobject_cast<QFileSystemModel *>(sourceModel());
        const bool asc = sortOrder() == Qt::AscendingOrder;

        const QFileInfo leftFileInfo = fsm->fileInfo(source_left);
        const QFileInfo rightFileInfo = fsm->fileInfo(source_right);

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

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
