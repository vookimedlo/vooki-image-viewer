/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class ImageZoomTest: public QObject
{
    Q_OBJECT

private slots:
    void areaSize() const;
    void fitToArea() const;
    void originalImageSize() const;
    void scaleFactor() const;
    void resetProperties() const;
    void transform() const;
};
