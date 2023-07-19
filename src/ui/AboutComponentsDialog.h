#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include "ui_AboutComponentsDialog.h"
#include <QDialog>

class AboutComponentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutComponentsDialog(QWidget *parent = Q_NULLPTR);
    DISABLE_COPY_MOVE(AboutComponentsDialog);

public Q_SLOTS:
    virtual void onSelectedComponentChanged(int row);

protected:
    void showResourceMarkdown(const QString &resource);

private:
    Ui::AboutComponentsDialog m_uiAboutComponentsDialog;
};
