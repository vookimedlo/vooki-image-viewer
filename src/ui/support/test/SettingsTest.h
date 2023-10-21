/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <QMenu>
#include <QSettings>
#include <queue>
#include <unordered_set>
#include "../SettingsStrings.h"


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
        std::queue<const QMenu *> unprocessedMenus;
        std::ranges::for_each(menus, [&unprocessedMenus](const QMenu * const menu){
            unprocessedMenus.push(menu);
        });

        while(!unprocessedMenus.empty())
        {
            const QMenu *const menu = unprocessedMenus.front();
            unprocessedMenus.pop();

            for (const auto *const action : menu->actions())
            {
                if (action->isSeparator())
                    continue;

                if (action->menu() && action->menu() != menu)
                {
                    unprocessedMenus.push(action->menu());
                    continue;
                }

                if (action->whatsThis().isEmpty())
                    continue;

                settings.setValue(action->whatsThis(), action->shortcut());
            }
        }
    }


private slots:
    void initializeSettings() const;
    void initializeSettingsByMenu() const;
};
