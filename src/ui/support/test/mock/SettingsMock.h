#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE
****************************************************************************/


#include "../../Settings.h"

class SettingsMock: public Settings
{
public:
    SettingsMock() = delete;
    DISABLE_COPY_MOVE(SettingsMock);

    static void initializeSettings(QSettings &defaultSettings)
    {
        Settings::initializeSettings(&defaultSettings);
    }

    static void initializeSettings(const QMenu *menu, QSettings &defaultSettings, const QSettings &userSettings)
    {
        Settings::initializeSettings(menu, &defaultSettings, &userSettings);
    }

    static void restoreDefaultSettings(const QSettings &defaultSettings, QSettings &userSettings)
    {
        Settings::restoreDefaultSettings(&defaultSettings, &userSettings);
    }
};
