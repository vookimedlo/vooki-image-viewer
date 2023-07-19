/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2019 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../darkmode.h"
#include <QLabel>

namespace SystemDependant
{
    QPixmap darkModePixmap(const QString &fileName)
    {
        if (isDarkMode())
        {
            return { fileName + "-white" };
        }
        else
        {
            return { fileName };
        }
    }

    bool isDarkMode()
    {
        QLabel label(".");
        return label.palette().color(QPalette::WindowText).value() > label.palette().color(QPalette::Window).value();
    }
}
