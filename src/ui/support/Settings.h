#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../util/compiler.h"
#include <QMenu>
#include <QSettings>
#include <memory>

class Settings
{
public:
    Settings() = delete;
    DISABLE_COPY_MOVE(Settings);

    [[nodiscard]] static std::unique_ptr<QSettings> defaultSettings();
    static void initializeSettings();
    static void initializeSettings(const QMenu *menu);
    [[nodiscard]] static std::unique_ptr<QSettings> userSettings();

protected:
    static void initializeSettings(QSettings &defaultSettings);
    static void initializeSettings(const QMenu *menu, QSettings &defaultSettings, const QSettings &userSettings);
    static void restoreDefaultSettings(const QSettings &defaultSettings, QSettings &userSettings);

private:
    static void restoreDefaultSettings();
};
