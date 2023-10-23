/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

class SettingsDialogTest: public QObject
{
    Q_OBJECT

public:
    explicit SettingsDialogTest(QObject *parent = nullptr) : QObject(parent){
        QCoreApplication::setOrganizationName("this is a testing organization");
    }

private slots:
    void populateShortcuts() const;
};
