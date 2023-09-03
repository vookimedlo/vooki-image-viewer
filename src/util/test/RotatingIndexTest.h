/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class RotatingIndexTest: public QObject
{
    Q_OBJECT

private slots:
    void postDecrement4x();
    void preDecrement4x();

    void postIncrement4x();
    void preIncrement4x();
};
