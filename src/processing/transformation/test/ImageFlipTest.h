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
    void flipHorizontally2x() const;
    void flipVertically2x() const;
    void setFlipHorizontallyTrueFalse() const;
    void setFlipVerticallyTrueFalse() const;
    void setFlipHorizontallyFalse() const;
    void setFlipVerticallyFalse() const;
    void resetProperties() const;
};
