/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "SettingsTest.h"

#include "../Settings.h"

class SettingsMock: public Settings
{
public:
    SettingsMock() = delete;
    DISABLE_COPY_MOVE(SettingsMock);

    static void initializeSettings(QSettings &defaultSettings)
    {
        Settings::initializeSettings(defaultSettings);
    }

    static void initializeSettings(const QMenu *menu, QSettings &defaultSettings, const QSettings &userSettings)
    {
        Settings::initializeSettings(menu, defaultSettings, userSettings);
    }

    static void restoreDefaultSettings(const QSettings &defaultSettings, QSettings &userSettings)
    {
        Settings::restoreDefaultSettings(defaultSettings, userSettings);
    }
};

static bool readImaginaryFile([[maybe_unused]] QIODevice &device, [[maybe_unused]] QSettings::SettingsMap &map)
{
    return true;
}

static bool writeImaginaryFile([[maybe_unused]] QIODevice &device, [[maybe_unused]] const QSettings::SettingsMap &map)
{
    return true;
}

void SettingsTest::initializeSettings() const
{
    const QSettings::Format nullFormat = QSettings::registerFormat("null", readImaginaryFile, writeImaginaryFile);
    QSettings settings(nullFormat, QSettings::UserScope, "test", "test");
    QCOMPARE(settings.allKeys().size(), 0);

    SettingsMock::initializeSettings(settings);
    const auto allKeys = settings.allKeys();
    QCOMPARE(allKeys.size(), m_expectedKeys.size());
    const std::unordered_set<QString> settingsKeys {allKeys.begin(), allKeys.end()};
    QCOMPARE(settingsKeys, m_expectedKeys);
}
