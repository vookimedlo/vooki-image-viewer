#pragma once
/****************************************************************************
VookiImageViewer - tool to showing images.
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

#include <memory>
#include <QColor>
#include <QDialog>
#include <QMenu>
#include <QSettings>
#include "ui_SettingsDialog.h"
#include "../util/compiler.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = Q_NULLPTR);
    DISABLE_COPY_MOVE(SettingsDialog);

    void populateShortcuts(QMenu *menu);

protected:
    void initializeUI(std::shared_ptr<QSettings> settings);

protected slots:
    virtual void onAccept();
    virtual void onButtonBoxButtonClicked(QAbstractButton *button);
    virtual void onRejected();
    virtual void onRestoreDefaultsTriggered();
    virtual void onToolButtonBorderColorClicked();
    virtual void onToolButtonBackgroundColorClicked();

private:
    Ui::SettingsDialog m_uiSettingsDialog;
    QColor m_borderColor;
    QColor m_backgroundColor;
};
