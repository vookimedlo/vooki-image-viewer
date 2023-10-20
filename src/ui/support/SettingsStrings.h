#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <unordered_set>

#define FOR_LIST_OF_VARIABLES(ITEM) \
    ITEM(SETTINGS_GENERAL_FULLSCREEN, "viv/general/fullscreen") \
    ITEM(SETTINGS_FULLSCREEN_HIDE_STATUSBAR, "viv/fullscreen/hide/statusbar") \
    ITEM(SETTINGS_FULLSCREEN_HIDE_TOOLBAR, "viv/fullscreen/hide/toolbar") \
    ITEM(SETTINGS_FULLSCREEN_HIDE_NAVIGATION, "viv/fullscreen/hide/navigation") \
    ITEM(SETTINGS_FULLSCREEN_HIDE_INFORMATION, "viv/fullscreen/hide/information") \
    ITEM(SETTINGS_IMAGE_REMEMBER_RECENT, "viv/image/remember/recent") \
    ITEM(SETTINGS_IMAGE_FITIMAGETOWINDOW, "viv/image/fitimagetowindow") \
    ITEM(SETTINGS_IMAGE_BORDER_DRAW, "viv/image/border/draw") \
    ITEM(SETTINGS_IMAGE_BORDER_COLOR, "viv/image/border/color") \
    ITEM(SETTINGS_IMAGE_BACKGROUND_COLOR, "viv/image/background/color") \
    ITEM(SETTINGS_LANGUAGE_USE_SYSTEM, "viv/language/system") \
    ITEM(SETTINGS_LANGUAGE_CODE, "viv/language/code") \
    ITEM(SETTINGS_RECENT_FILE_1, "viv/recent/file/1") \
    ITEM(SETTINGS_RECENT_FILE_2, "viv/recent/file/2") \
    ITEM(SETTINGS_RECENT_FILE_3, "viv/recent/file/3") \
    ITEM(SETTINGS_RECENT_FILE_4, "viv/recent/file/4") \
    ITEM(SETTINGS_RECENT_FILE_5, "viv/recent/file/5") \
    ITEM(SETTINGS_WINDOW_HIDE_STATUSBAR, "viv/window/hide/statusbar") \
    ITEM(SETTINGS_WINDOW_HIDE_TOOLBAR, "viv/window/hide/toolbar") \
    ITEM(SETTINGS_WINDOW_HIDE_NAVIGATION, "viv/window/hide/navigation") \
    ITEM(SETTINGS_WINDOW_HIDE_INFORMATION, "viv/window/hide/information") \

#define DEFINE_VARIABLES(variable, value, ...) static constexpr auto const variable = value;
FOR_LIST_OF_VARIABLES( DEFINE_VARIABLES )
#undef DEFINE_VARIABLES

#define LIST_VARIABLES(variable, value, ...) variable,
static const std::unordered_set<const char *> settingsSet {FOR_LIST_OF_VARIABLES( LIST_VARIABLES )};
#undef LIST_VARIABLES
