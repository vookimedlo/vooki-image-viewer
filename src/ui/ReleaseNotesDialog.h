#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include "ui_ReleaseNotesDialog.h"
#include <QDialog>

class ReleaseNotesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReleaseNotesDialog(QWidget *parent = Q_NULLPTR);
    DISABLE_COPY_MOVE(ReleaseNotesDialog);

private:
    Ui::releaseNotesDialog m_uiReleaseNotesDialog;
};
