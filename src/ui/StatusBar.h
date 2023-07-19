#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include <QFrame>
#include <QLabel>
#include <QStatusBar>
#include <memory>

class StatusBar : public QStatusBar
{
public:
    explicit StatusBar(QWidget *parent = nullptr);
    DISABLE_COPY_MOVE(StatusBar);

    void setDimensionLabel(const QString &value);
    void setSizeLabel(const QString &value);
    void setZoomLabel(const QString &value);

    void clearLabels();

protected:
    std::unique_ptr<QFrame> createVerticalLine();

private:
    QLabel m_dimensionsLabel{this};
    QLabel m_sizeLabel{this};
    QLabel m_zoomLabel{this};
};
