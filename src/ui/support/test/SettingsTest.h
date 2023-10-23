/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <QSettings>
#include <unordered_set>
#include "../SettingsStrings.h"
#include "../../../util/testing.h"


class SettingsTest: public QObject
{
    Q_OBJECT

    const std::unordered_set<QString> m_expectedKeys =
        []() {
            std::unordered_set<QString> expectedKeys;
            std::ranges::for_each(settingsSet, [&expectedKeys](const char *string) { expectedKeys.emplace(string); });
            return expectedKeys;
        }();

    template <typename T, size_t N>
    void getAllShortcuts(QSettings &settings, const std::array<T,N>& menus) const
    {
        for (const auto action : TEST::getAllActionsHavingShortcut(menus))
            settings.setValue(action->whatsThis(), action->shortcut());
    }

public:
    explicit SettingsTest(QObject *parent = nullptr) : QObject(parent){
        QCoreApplication::setOrganizationName("this is testing organization");
    }

private slots:
    void initializeSettings() const;
    void initializeSettingsByMenu() const;
    void getDefaultSettings() const;
    void getUserSettings() const;
};
