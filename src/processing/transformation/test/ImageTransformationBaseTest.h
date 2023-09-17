/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>


class ImageTransformationBaseTest: public QObject
{
    Q_OBJECT

private slots:
    void setCachedObjectTransform() const;
    void setCachedObjectImage() const;

    void invalidateCacheTransform() const;
    void invalidateCacheImage() const;

    void bindTransform() const;
    void bindImage() const;
};
