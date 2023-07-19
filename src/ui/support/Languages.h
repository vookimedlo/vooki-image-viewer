#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../util/compiler.h"
#include <QString>
#include <vector>

class Languages
{
public:
    Languages() = delete;
    DISABLE_COPY_MOVE(Languages);

    struct Record
    {
        QString m_code;
        QString m_language;
    };

    static const std::vector<Record> m_localizations;
};
