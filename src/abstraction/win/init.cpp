/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../init.h"
#include "../darkmode.h"
#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QStyleFactory>

namespace SystemDependant
{
    void Init()
    {
        if (isDarkMode())
        {
            // Taken from: https://forum.qt.io/topic/101391/windows-10-dark-theme/4
            QApplication::setStyle(QStyleFactory::create("Fusion"));
            QPalette darkPalette;
            auto darkColor = QColor(45, 45, 45);
            auto disabledColor = QColor(127, 127, 127);
            darkPalette.setColor(QPalette::Window, darkColor);
            darkPalette.setColor(QPalette::WindowText, Qt::white);
            darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
            darkPalette.setColor(QPalette::AlternateBase, darkColor);
            darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
            darkPalette.setColor(QPalette::ToolTipText, Qt::white);
            darkPalette.setColor(QPalette::Text, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
            darkPalette.setColor(QPalette::Button, darkColor);
            darkPalette.setColor(QPalette::ButtonText, Qt::white);
            darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

            darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

            QApplication::setPalette(darkPalette);

            qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #111111; border: 1px solid #C0C0C0; }");
        }
    }
}
