/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "SettingsTest.h"

#include "mock/QSettingsMock.h"
#include "mock/SettingsMock.h"
#include "mock/ui/MainWindow.h"

void SettingsTest::initializeSettings() const
{
    QSettings settings(QSettingsMock::nullFormat, QSettings::UserScope, "test", "test");
    QCOMPARE(settings.allKeys().size(), 0);

    SettingsMock::initializeSettings(settings);
    const auto allKeys = settings.allKeys();
    QCOMPARE(allKeys.size(), m_expectedKeys.size());
    const std::unordered_set<QString> settingsKeys {allKeys.begin(), allKeys.end()};
    QCOMPARE(settingsKeys, m_expectedKeys);
}

void SettingsTest::initializeSettingsByMenu() const
{
    MainWindow mainWindow(nullptr);
    const auto allMenus = mainWindow.allMenus();

    QSettings defaultSettings(QSettingsMock::nullFormat, QSettings::UserScope, "test", "test");
    QSettings userSettings(QSettingsMock::nullFormat, QSettings::UserScope, "test2", "test2");

    QSettings settingsBeforeInitialization(QSettingsMock::nullFormat, QSettings::UserScope, "test3", "test3");
    getAllShortcuts(settingsBeforeInitialization, allMenus);

    for (const auto menu : allMenus)
    {
        SettingsMock::initializeSettings(menu, defaultSettings, userSettings);
        const auto allKeys = defaultSettings.allKeys();
        std::ranges::for_each(allKeys, [](const QString &string){ QCOMPARE(string.startsWith("viv/shortcut/", Qt::CaseSensitive), true); });
    }

    QVERIFY(defaultSettings.allKeys().size() > 0);
    QCOMPARE(userSettings.allKeys().size(), 0);

    QSettings settingsAfterInitialization(QSettingsMock::nullFormat, QSettings::UserScope, "test4", "test4");
    getAllShortcuts(settingsAfterInitialization, allMenus);

    auto allKeysBeforeInitialization {settingsBeforeInitialization.allKeys()};
    auto allKeysAfterInitialization {settingsAfterInitialization.allKeys()};
    allKeysBeforeInitialization.sort();
    allKeysAfterInitialization.sort();

    QCOMPARE(allKeysAfterInitialization, allKeysBeforeInitialization);
    for (const auto &key : allKeysBeforeInitialization)
        QCOMPARE(settingsAfterInitialization.value(key).value<QKeySequence>(), settingsBeforeInitialization.value(key).value<QKeySequence>());

    // Test if the user settings is propagated to the MainWindow menu items
    std::ranges::for_each(allKeysBeforeInitialization, [&userSettings](const auto &key) {
        static int i = QKeySequence::HelpContents;
        userSettings.setValue(key, QKeySequence(static_cast<QKeySequence::StandardKey>(i++)));
    });

    QCOMPARE(userSettings.allKeys().size(), defaultSettings.allKeys().size());

    for (const auto menu : allMenus)
    {
        SettingsMock::initializeSettings(menu, defaultSettings, userSettings);
        const auto allKeys = defaultSettings.allKeys();
        std::ranges::for_each(allKeys, [](const QString &string){ QCOMPARE(string.startsWith("viv/shortcut/", Qt::CaseSensitive), true); });
    }

    QCOMPARE(userSettings.allKeys().size(), defaultSettings.allKeys().size());

    QSettings settingsAfterUserInitialization(QSettingsMock::nullFormat, QSettings::UserScope, "test5", "test5");
    getAllShortcuts(settingsAfterUserInitialization, allMenus);

    auto allKeysUserSettings {userSettings.allKeys()};
    auto allKeysAfterUserInitialization {settingsAfterUserInitialization.allKeys()};
    allKeysUserSettings.sort();
    allKeysAfterUserInitialization.sort();

    QCOMPARE(allKeysUserSettings, allKeysAfterUserInitialization);
    for (const auto &key : allKeysUserSettings)
        QCOMPARE(settingsAfterUserInitialization.value(key).value<QKeySequence>(), userSettings.value(key).value<QKeySequence>());
}

void SettingsTest::getDefaultSettings() const
{
    const auto defaultSettings = SettingsMock::defaultSettings();
    QCOMPARE(defaultSettings->applicationName(), "");
    QCOMPARE(defaultSettings->organizationName(), QCoreApplication::organizationName());
}

void SettingsTest::getUserSettings() const
{
    const auto userSettings = SettingsMock::userSettings();
    QCOMPARE(userSettings->applicationName(), QCoreApplication::applicationName());
    QCOMPARE(userSettings->organizationName(), QCoreApplication::organizationName());
}
