#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#define DISABLE_COPY_MOVE(CLASS)                                                                                                                               \
    CLASS &operator=(const CLASS &) = delete;                                                                                                                  \
    CLASS(const CLASS &) = delete;                                                                                                                             \
    CLASS &operator=(const CLASS &&) = delete;                                                                                                                 \
    CLASS(const CLASS &&) = delete
