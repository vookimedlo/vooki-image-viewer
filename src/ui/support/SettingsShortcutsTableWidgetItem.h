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
#include <QKeySequence>
#include <QObject>
#include <QTableWidgetItem>

class SettingsShortcutsTableWidgetItem final
                                        : public QObject
                                        , public QTableWidgetItem
{
    Q_OBJECT

public:
    explicit SettingsShortcutsTableWidgetItem(QAction &action);
    DISABLE_COPY_MOVE(SettingsShortcutsTableWidgetItem);

    [[nodiscard]] QAction &action() const;
    [[nodiscard]] QKeySequence keySequence() const;

    static const int type;

public Q_SLOTS:
    void onKeySequenceChanged(const QKeySequence &keySequence);

private:
    QAction &m_action;
    QKeySequence m_keySequence;
};
