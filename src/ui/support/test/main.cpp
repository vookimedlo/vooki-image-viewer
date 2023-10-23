/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../../util/testing.h"

#include "RecentFileActionTest.h"
#include "SettingsTest.h"
#include "SettingsShortcutsTableWidgetItemTest.h"


int main(int argc, char *argv[])
{
    int status = 0;

    TEST::runTests<RecentFileActionTest>(argc, argv, &status);
    TEST::runTests<SettingsTest>(argc, argv, &status);
    TEST::runTests<SettingsShortcutsTableWidgetItemTest>(argc, argv, &status);

    return status;
}
