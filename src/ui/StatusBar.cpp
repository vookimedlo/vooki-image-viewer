/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "StatusBar.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    addPermanentWidget(&m_dimensionsLabel);
    addPermanentWidget(createVerticalLine().release());
    addPermanentWidget(&m_sizeLabel);
    addPermanentWidget(createVerticalLine().release());
    addPermanentWidget(&m_zoomLabel);

    m_dimensionsLabel.setToolTip(tr("Image dimensions"));
    m_sizeLabel.setToolTip(tr("Image size"));
    m_zoomLabel.setToolTip(tr("Zoom"));
}

std::unique_ptr<QFrame> StatusBar::createVerticalLine()
{
    auto line = std::make_unique<QFrame>(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(320, 150, 118, 3));
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

void StatusBar::setDimensionLabel(const QString &value)
{
    m_dimensionsLabel.setText(value);
}

void StatusBar::setSizeLabel(const QString &value)
{
    m_sizeLabel.setText(value);
}

void StatusBar::setZoomLabel(const QString &value)
{
    m_zoomLabel.setText(value);
}

void StatusBar::clearLabels()
{
    showMessage("");
    m_sizeLabel.clear();
    m_dimensionsLabel.clear();
    m_zoomLabel.clear();
}
