#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../../../../util/compiler.h"
#include <QTreeView>

class FileSystemTreeView final : public QTreeView
{
    Q_OBJECT

public:
    explicit FileSystemTreeView(QWidget *parent = nullptr) : QTreeView(parent) {};
    DISABLE_COPY_MOVE(FileSystemTreeView);

public slots:
    void setCurrentIndex([[maybe_unused]] const QModelIndex &index) {};

private slots:
    void onExpanded([[maybe_unused]] const QModelIndex &index) {};
};
