/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
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
