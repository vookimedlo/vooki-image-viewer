/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2019 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../darkmode.h"
#import <AppKit/NSWindow.h>

namespace SystemDependant
{
    QPixmap darkModePixmap(const QString &fileName)
    {
        if(isDarkMode()) {
            return {fileName + "-white"};
        }
        else {
            return {fileName};
        }
    }

    bool isDarkMode()
    {
        NSAppearance *appearance = NSAppearance.currentDrawingAppearance;
        return appearance.name == NSAppearanceNameDarkAqua;
    }
}
