/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class ImageFlipTest: public QObject
{
    Q_OBJECT

private slots:
    void flipHorizontally2x();
    void flipVertically2x();
    void setFlipHorizontallyTrueFalse();
    void setFlipVerticallyTrueFalse();
    void setFlipHorizontallyFalse();
    void setFlipVerticallyFalse();
    void resetProperties();
};
