/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class ImageBorderTest: public QObject
{
    Q_OBJECT

private slots:
    void areaSize() const;
    void borderColor() const;
    void backgroundColor() const;
    void drawBorder() const;
    void imageOffsets() const;
    void resetProperties() const;
};
