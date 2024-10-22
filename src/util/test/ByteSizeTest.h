/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class ByteSizeTest: public QObject
{
    Q_OBJECT

private slots:
    void humanReadableSizeForValue1() const;
    void humanReadableSizeForEdgeValueMinus1() const;
    void humanReadableSizeForEdgeValuePlus1() const;
};
