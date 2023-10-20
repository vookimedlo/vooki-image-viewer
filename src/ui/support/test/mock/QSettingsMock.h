#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE
****************************************************************************/

#include <QSettings>

class QSettingsMock
{
    static bool readImaginaryFile([[maybe_unused]] QIODevice &device, [[maybe_unused]] QSettings::SettingsMap &map)
    {
        return true;
    }

    static bool writeImaginaryFile([[maybe_unused]] QIODevice &device, [[maybe_unused]] const QSettings::SettingsMap &map)
    {
        return true;
    }

public:
    static const QSettings::Format nullFormat;
};
