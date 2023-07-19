/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "RecentFileAction.h"

RecentFileAction::RecentFileAction(const QString &text, QObject *parent)
                                        : QAction(text, parent)
{
    QObject::connect(this, &QAction::triggered, this, &RecentFileAction::actionTriggered);
}

RecentFileAction::~RecentFileAction()
{
    QObject::disconnect(this, &QAction::triggered, this, &RecentFileAction::actionTriggered);
}

void RecentFileAction::actionTriggered()
{
    emit recentFileActionTriggered(text());
}
