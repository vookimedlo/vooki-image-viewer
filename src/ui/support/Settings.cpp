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
#include "SettingsStrings.h"

void Settings::initializeSettings()
{
    QSettings defaultSettings(QSettings::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    defaultSettings.setValue(SETTINGS_GENERAL_FULLSCREEN, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_STATUSBAR, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_TOOLBAR, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_NAVIGATION, false);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_STATUSBAR, true);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_TOOLBAR, true);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_NAVIGATION, true);
    defaultSettings.setValue(SETTINGS_IMAGE_REMEMBER_RECENT, true);
    defaultSettings.setValue(SETTINGS_IMAGE_FITIMAGETOWINDOW, false);
    defaultSettings.setValue(SETTINGS_IMAGE_BORDER_DRAW, false);
    defaultSettings.setValue(SETTINGS_IMAGE_BORDER_COLOR, QColor(Qt::white));
    defaultSettings.setValue(SETTINGS_IMAGE_BACKGROUND_COLOR, QColor(Qt::black));
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
