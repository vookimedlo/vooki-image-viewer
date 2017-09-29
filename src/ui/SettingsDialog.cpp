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

#include "SettingsDialog.h"

#include <QColorDialog>
#include <QSettings>
#include "support/SettingsShortcutsTableWidgetItem.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), m_borderColor(), m_backgroundColor()
{
    m_uiSettingsDialog.setupUi(this);
    QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->setChecked(settings.value("general/fullscreen", false).toBool());
    m_uiSettingsDialog.checkBoxWindowHideStatusbar->setChecked(settings.value("window/hide/statusbar", false).toBool());
    m_uiSettingsDialog.checkBoxWindowHideToolbar->setChecked(settings.value("window/hide/toolbar", false).toBool());
    m_uiSettingsDialog.checkBoxWindowHideNavigation->setChecked(settings.value("window/hide/navigation", false).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->setChecked(settings.value("fullscreen/hide/statusbar", true).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideToolbar->setChecked(settings.value("fullscreen/hide/toolbar", true).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideNavigation->setChecked(settings.value("fullscreen/hide/navigation", true).toBool());
    m_uiSettingsDialog.checkBoxRemeberRecentImages->setChecked(settings.value("image/remember/recent", true).toBool());
    m_uiSettingsDialog.checkBoxImageFitToWindow->setChecked(settings.value("image/fitimagetowindow", false).toBool());
    m_uiSettingsDialog.checkBoxImageDrawBorder->setChecked(settings.value("image/border/draw", false).toBool());
    m_uiSettingsDialog.toolButtonBorderColor->setEnabled(settings.value("image/border/draw", false).toBool());
    m_borderColor = settings.value("image/border/color", QColor(Qt::white)).value<QColor>();
    m_backgroundColor = settings.value("image/background/color", QColor(Qt::black)).value<QColor>();
}

void SettingsDialog::populateShortcuts(QMenu *menu)
{
    auto actions = menu->actions();
    for (QAction *action : actions)
    {
        if (action->isSeparator())
            continue;

        // I don't like recursion, but menus are usually not too nested, so it doesn't matter.
        if (action->menu() && action->menu() != menu)
        {
            populateShortcuts(action->menu());
            continue;
        }

        int rowCount = m_uiSettingsDialog.tableShortcutsWidget->rowCount();
        m_uiSettingsDialog.tableShortcutsWidget->insertRow(rowCount);
        QTableWidgetItem *headerItem = new QTableWidgetItem(action->toolTip());
        m_uiSettingsDialog.tableShortcutsWidget->setVerticalHeaderItem(rowCount, headerItem);

        SettingsShortcutsTableWidgetItem *item = new SettingsShortcutsTableWidgetItem(*action);
        m_uiSettingsDialog.tableShortcutsWidget->setItem(rowCount, 0, item);
    }
}

void SettingsDialog::onAccept()
{
    QSettings settings(QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("general/fullscreen", m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->isChecked());
    settings.setValue("window/hide/statusbar", m_uiSettingsDialog.checkBoxWindowHideStatusbar->isChecked());
    settings.setValue("window/hide/toolbar", m_uiSettingsDialog.checkBoxWindowHideToolbar->isChecked());
    settings.setValue("window/hide/navigation", m_uiSettingsDialog.checkBoxWindowHideNavigation->isChecked());
    settings.setValue("fullscreen/hide/statusbar", m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->isChecked());
    settings.setValue("fullscreen/hide/toolbar", m_uiSettingsDialog.checkBoxFullscreenHideToolbar->isChecked());
    settings.setValue("fullscreen/hide/navigation", m_uiSettingsDialog.checkBoxFullscreenHideNavigation->isChecked());
    settings.setValue("image/remember/recent", m_uiSettingsDialog.checkBoxRemeberRecentImages->isChecked());
    settings.setValue("image/fitimagetowindow", m_uiSettingsDialog.checkBoxImageFitToWindow->isChecked());
    settings.setValue("image/border/draw", m_uiSettingsDialog.checkBoxImageDrawBorder->isChecked());
    settings.setValue("image/border/color", m_borderColor);
    settings.setValue("image/background/color", m_backgroundColor);

    // store all shortcuts in user settings
    for(int i = 0; i < m_uiSettingsDialog.tableShortcutsWidget->rowCount(); i++)
    {
        QTableWidgetItem *item = m_uiSettingsDialog.tableShortcutsWidget->item(i, 0);

        if (item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            SettingsShortcutsTableWidgetItem *shortcutItem = static_cast<SettingsShortcutsTableWidgetItem*>(item);
            settings.setValue(shortcutItem->action().whatsThis(), shortcutItem->keySequence());
        }
    }

    QDialog::accept();
}

void SettingsDialog::onButtonBoxButtonClicked(QAbstractButton *button)
{
    switch(m_uiSettingsDialog.buttonBox->buttonRole(button))
    {
    case QDialogButtonBox::ButtonRole::ResetRole:
        onRestoreDefaultsTriggered();
        break;
    default:
        return;
    }
}

void SettingsDialog::onRestoreDefaultsTriggered()
{
    m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->setChecked(false);
    m_uiSettingsDialog.checkBoxWindowHideStatusbar->setChecked(false);
    m_uiSettingsDialog.checkBoxWindowHideToolbar->setChecked(false);
    m_uiSettingsDialog.checkBoxWindowHideNavigation->setChecked(false);
    m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->setChecked(true);
    m_uiSettingsDialog.checkBoxFullscreenHideToolbar->setChecked(true);
    m_uiSettingsDialog.checkBoxFullscreenHideNavigation->setChecked(true);
    m_uiSettingsDialog.checkBoxRemeberRecentImages->setChecked(true);
    m_uiSettingsDialog.checkBoxImageFitToWindow->setChecked(false);
    m_uiSettingsDialog.checkBoxImageDrawBorder->setChecked(false);
    m_uiSettingsDialog.toolButtonBorderColor->setEnabled(false);
    m_borderColor = Qt::white;
    m_backgroundColor = Qt::black;
}

void SettingsDialog::onToolButtonBorderColorClicked()
{
    QColor borderColor = QColorDialog::getColor(m_borderColor);
    if (borderColor.isValid())
        m_borderColor = borderColor;
}

void SettingsDialog::onToolButtonBackgroundColorClicked()
{
    QColor backgroundColor = QColorDialog::getColor(m_backgroundColor);
    if (backgroundColor.isValid())
        m_backgroundColor = backgroundColor;
}
