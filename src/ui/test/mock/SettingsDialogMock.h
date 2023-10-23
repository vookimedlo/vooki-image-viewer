#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../../../util/compiler.h"
#include <memory>
#include "../../SettingsDialog.h"

class SettingsDialogMock final : public SettingsDialog
{
public:
    using SettingsDialog::SettingsDialog;
    DISABLE_COPY_MOVE(SettingsDialogMock);

    const Ui::SettingsDialog *dialog() { return &m_uiSettingsDialog; }
};
