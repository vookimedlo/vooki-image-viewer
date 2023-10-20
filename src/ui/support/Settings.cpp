/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "Settings.h"
#include "SettingsStrings.h"
#include <QCoreApplication>

std::unique_ptr<QSettings> Settings::defaultSettings()
{
    return std::make_unique<QSettings>(QSettings::UserScope, QCoreApplication::organizationName());
}

void Settings::initializeSettings()
{
    QSettings defaultSettings(QSettings::UserScope, QCoreApplication::organizationName());
    Settings::initializeSettings(defaultSettings);
}

void Settings::initializeSettings(QSettings &defaultSettings)
{
    defaultSettings.setValue(SETTINGS_GENERAL_FULLSCREEN, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_STATUSBAR, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_TOOLBAR, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_NAVIGATION, false);
    defaultSettings.setValue(SETTINGS_WINDOW_HIDE_INFORMATION, false);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_STATUSBAR, true);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_TOOLBAR, true);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_NAVIGATION, true);
    defaultSettings.setValue(SETTINGS_FULLSCREEN_HIDE_INFORMATION, true);
    defaultSettings.setValue(SETTINGS_IMAGE_REMEMBER_RECENT, true);
    defaultSettings.setValue(SETTINGS_IMAGE_FITIMAGETOWINDOW, false);
    defaultSettings.setValue(SETTINGS_IMAGE_BORDER_DRAW, false);
    defaultSettings.setValue(SETTINGS_IMAGE_BORDER_COLOR, QColor(Qt::white));
    defaultSettings.setValue(SETTINGS_IMAGE_BACKGROUND_COLOR, QColor(Qt::black));
    defaultSettings.setValue(SETTINGS_LANGUAGE_USE_SYSTEM, true);
    defaultSettings.setValue(SETTINGS_LANGUAGE_CODE, QString("en_US"));

    defaultSettings.setValue(SETTINGS_RECENT_FILE_1, QString());
    defaultSettings.setValue(SETTINGS_RECENT_FILE_2, QString());
    defaultSettings.setValue(SETTINGS_RECENT_FILE_3, QString());
    defaultSettings.setValue(SETTINGS_RECENT_FILE_4, QString());
    defaultSettings.setValue(SETTINGS_RECENT_FILE_5, QString());
}

void Settings::initializeSettings(const QMenu *menu)
{
    QSettings defaultSettings(QSettings::UserScope, QCoreApplication::organizationName());
    QSettings userSettings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    Settings::initializeSettings(menu, defaultSettings, userSettings);
}

void Settings::initializeSettings(const QMenu *menu, QSettings &defaultSettings, const QSettings &userSettings)
{
    auto actions = menu->actions();
    for (auto * const action : actions)
    {
        if (action->isSeparator())
            continue;

        // I don't like recursion, but menus are usually not too nested, so it doesn't matter.
        if (action->menu() && action->menu() != menu)
        {
            Settings::initializeSettings(action->menu(), defaultSettings, userSettings);
            continue;
        }

        if (action->whatsThis().isEmpty())
            continue;

        defaultSettings.setValue(action->whatsThis(), action->shortcut());
        action->setShortcut(userSettings.value(action->whatsThis(), defaultSettings.value(action->whatsThis())).value<QKeySequence>());
    }
}

void Settings::restoreDefaultSettings()
{
    QSettings defaultSettings(QSettings::UserScope, QCoreApplication::organizationName());
    QSettings userSettings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    Settings::restoreDefaultSettings(defaultSettings, userSettings);
}

void Settings::restoreDefaultSettings(const QSettings &defaultSettings, QSettings &userSettings)
{
    for (const QString &key : defaultSettings.allKeys())
        userSettings.setValue(key, defaultSettings.value(key));
}

std::unique_ptr<QSettings> Settings::userSettings()
{
    return std::make_unique<QSettings>(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
}
