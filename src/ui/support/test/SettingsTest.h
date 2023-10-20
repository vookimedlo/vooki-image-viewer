/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <unordered_set>
#include "../SettingsStrings.h"


class SettingsTest: public QObject
{
    Q_OBJECT

    const std::unordered_set<QString> m_expectedKeys =
        []() {
            std::unordered_set<QString> expectedKeys;
            std::ranges::for_each(settingsSet, [&expectedKeys](const char *string) { expectedKeys.emplace(QString(string)); });
            return expectedKeys;
        }();


private slots:
    void initializeSettings() const;
};
