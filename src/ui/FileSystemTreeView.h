#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include <QTreeView>

class FileSystemTreeView final : public QTreeView
{
    Q_OBJECT

public:
    explicit FileSystemTreeView(QWidget *parent = nullptr);
    ~FileSystemTreeView() override;
    DISABLE_COPY_MOVE(FileSystemTreeView);

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void setCurrentIndex(const QModelIndex &index);

private slots:
    void onExpanded(const QModelIndex &index);

private:
    QModelIndex m_setIndex;
};
