#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include <QSortFilterProxyModel>

class FileSystemSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit FileSystemSortFilterProxyModel(QObject *parent = nullptr);
    DISABLE_COPY_MOVE(FileSystemSortFilterProxyModel);

protected:
    [[nodiscard]] bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};
