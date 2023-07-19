/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2019 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../darkmode.h"
#include <QSettings>

namespace SystemDependant
{
    static const QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);

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
        static const bool result = settings.contains("AppsUseLightTheme") && settings.contains("AppsUseLightTheme") != 0;
        return result;
    }
}
