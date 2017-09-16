#include "../init.h"

#import <AppKit/NSWindow.h>

namespace SystemDependant
{
    void Init()
    {
        // Disable unwanted MacOS menu items.
        // - View -> Enter/Exit Fullscreen
        // - Edit -> Start Dictation
        // - Edit -> Emoji & Symbols
        
        [[NSUserDefaults standardUserDefaults] setBool:NO  forKey:@"NSFullScreenMenuItemEverywhere"];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledDictationMenuItem"];
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSDisabledCharacterPaletteMenuItem"];
    }
}
