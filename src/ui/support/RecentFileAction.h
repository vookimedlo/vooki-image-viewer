#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../util/compiler.h"
#include <QAction>

class RecentFileAction : public QAction
{
    Q_OBJECT

public:
    explicit RecentFileAction(const QString &text, QObject *parent = nullptr);
    ~RecentFileAction() override;
    DISABLE_COPY_MOVE(RecentFileAction);

signals:
    void recentFileActionTriggered(const QString &filename);

private slots:
    void actionTriggered();
};
