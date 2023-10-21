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
    using QTreeView::QTreeView;
    DISABLE_COPY_MOVE(FileSystemTreeView);
};
