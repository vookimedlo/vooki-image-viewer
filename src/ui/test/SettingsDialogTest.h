/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>
#include <QString>
#include <QKeySequence>
#include <unordered_set>

class SettingsDialogTest: public QObject
{
    Q_OBJECT

    struct TextShortcutPairHashFunction
    {
        size_t operator()(const std::pair<QString, QKeySequence> &x) const
        {
            return qHash((x.first + x.second.toString()), 1234);
        }
    };
    using ResultingSet = std::unordered_set<std::pair<QString, QKeySequence>, TextShortcutPairHashFunction>;

public:
    explicit SettingsDialogTest(QObject *parent = nullptr) : QObject(parent){
        QCoreApplication::setOrganizationName("this is a testing organization");
    }

private slots:
    void shortcuts() const;
};
