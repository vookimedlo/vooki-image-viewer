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

    using SettingsDialog::ui;

    bool m_pickerReturnsValidColor {true};
    const QColor m_pickerValidColor {Qt::darkBlue};

    [[nodiscard]] QSettings &getDefaultSettings() override { return SettingsDialog::getDefaultSettings(); };
    [[nodiscard]] QSettings &getUserSettings() override { return SettingsDialog::getUserSettings(); };

protected:
    QColor getColorFromPicker([[maybe_unused]] const QColor &initialColor) const override
    {
        return m_pickerReturnsValidColor ? m_pickerValidColor : QColor();
    }
};
