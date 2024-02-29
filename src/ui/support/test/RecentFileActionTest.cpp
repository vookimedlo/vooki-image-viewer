/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/


#include "RecentFileActionTest.h"

#include <QSignalSpy>
#include "../RecentFileAction.h"

void RecentFileActionTest::recentFileActionTriggered() const
{
    const QString expectedString("Some strange text ěíáčšýčšéřňŮ");
    RecentFileAction action {expectedString};
    QSignalSpy spy(&action, SIGNAL(recentFileActionTriggered(const QString &)));
    QCOMPARE(spy.count(), 0);

    action.trigger();
    QCOMPARE(spy.count(), 1);

    const QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).typeId(), QMetaType::QString);
    QCOMPARE(arguments.at(0).toString(), expectedString);
}
