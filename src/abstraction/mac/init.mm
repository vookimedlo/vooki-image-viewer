/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
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
