/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../init.h"

#import <AppKit/NSWindow.h>
#include "../../util/compiler.h"

namespace SystemDependant
{
    [[maybe_unused]] static void debugPrintNSUserDefaultsAllKeys()
    {
        NSLog(@"%@", [[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys]);
    }

    [[maybe_unused]] static void debugPrintNSUserDefaultsAllKeysAndValues()
    {
        NSLog(@"%@", [[NSUserDefaults standardUserDefaults] dictionaryRepresentation]);
    }

    void Init()
    {
        // Disable unwanted MacOS menu items.
        // - View -> Enter/Exit Fullscreen
        // - Edit -> Start Dictation
        // - Edit -> Emoji & Symbols

        [[NSUserDefaults standardUserDefaults] setBool:NO  forKey:@"NSFullScreenMenuItemEverywhere"];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledDictationMenuItem"];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledCharacterPaletteMenuItem"];

        // Disable unwanted MacOS menu items.
        // - View -> Show Tab Bar
        // - View -> Show All Tabs
        [NSWindow setAllowsAutomaticWindowTabbing: false];
    }
}
