/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2019  Michal Duda <github@vookimedlo.cz>

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

#include <QSettings>
#include "../darkmode.h"

namespace SystemDependant
{
    static QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);

    QPixmap darkModePixmap(const QString &fileName)
    {
        if(isDarkMode()) {
            return QPixmap(fileName + "-white");
        }
        else {
            return QPixmap(fileName);
        }
    }

    bool isDarkMode()
    {
        static const bool result = settings.contains("AppsUseLightTheme") && settings.contains("AppsUseLightTheme") != 0;
        return result;
    }
}
