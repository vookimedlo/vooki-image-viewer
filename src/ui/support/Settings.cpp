/****************************************************************************
VookiImageViewer - tool to showing images.
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

#include "Settings.h"
#include <QCoreApplication>

void Settings::initializeSettings()
{
    QSettings defaultSettings(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    defaultSettings.setValue("general/fullscreen", false);
    defaultSettings.setValue("window/hide/statusbar", false);
    defaultSettings.setValue("window/hide/toolbar", false);
    defaultSettings.setValue("window/hide/navigation", false);
    defaultSettings.setValue("fullscreen/hide/statusbar", true);
    defaultSettings.setValue("fullscreen/hide/toolbar", true);
    defaultSettings.setValue("fullscreen/hide/navigation", true);
    defaultSettings.setValue("image/remember/recent", true);
    defaultSettings.setValue("image/fitimagetowindow", false);
    defaultSettings.setValue("image/border/draw", false);
    defaultSettings.setValue("image/border/draw", false);
    defaultSettings.setValue("image/border/color", QColor(Qt::white));
    defaultSettings.setValue("image/background/color", QColor(Qt::black));
}

void Settings::initializeSettings(QMenu *menu)
{
    QSettings defaultSettings(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QSettings userSettings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    auto actions = menu->actions();
    for (QAction *action : actions)
    {
        if (action->isSeparator())
            continue;

        // I don't like recursion, but menus are usually not too nested, so it doesn't matter.
        if (action->menu() && action->menu() != menu)
        {
            initializeSettings(action->menu());
            continue;
        }

        defaultSettings.setValue(action->whatsThis(), action->shortcut());
        action->setShortcut(defaultSettings.value(action->whatsThis()).value<QKeySequence>());
    }
}

void Settings::restoreDefaultSettings()
{
    QSettings defaultSettings(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QSettings userSettings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    for (QString &key : defaultSettings.allKeys())
    {
        userSettings.setValue(key, defaultSettings.value(key));
    }
}

std::shared_ptr<QSettings> Settings::userSettings()
{
    return std::make_shared<QSettings>(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
}

std::shared_ptr<QSettings> Settings::defaultSettings()
{
    return std::make_shared<QSettings>(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
}
