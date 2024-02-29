/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class SettingsShortcutsTableWidgetItemTest: public QObject
{
    Q_OBJECT

signals:
    void keySequenceChanged(const QKeySequence &) const;

private slots:
    void onKeySequenceChanged() const;
};