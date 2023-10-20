/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE
****************************************************************************/

#include "QSettingsMock.h"

const QSettings::Format QSettingsMock::nullFormat {QSettings::registerFormat("null", QSettingsMock::readImaginaryFile, QSettingsMock::writeImaginaryFile)};
