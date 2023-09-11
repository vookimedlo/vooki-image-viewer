/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class ImageRotationTest: public QObject
{
    Q_OBJECT

private slots:
    void rotateLeft4x() const;
    void rotateRight4x() const;
    void rotateLeftRight() const;
    void rotateRightLeft() const;
    void resetProperties() const;
};
