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
#include "../ui/support/Settings.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), m_borderColor(), m_backgroundColor()
{
    m_uiSettingsDialog.setupUi(this);
    std::shared_ptr<QSettings> settings = Settings::userSettings();
    initializeUI(settings);
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

void SettingsDialog::initializeUI(std::shared_ptr<QSettings> settings)
{
    m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->setChecked(settings->value(m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideStatusbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideStatusbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideToolbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideToolbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideNavigation->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideNavigation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideToolbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideToolbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideNavigation->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideNavigation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxRemeberRecentImages->setChecked(settings->value(m_uiSettingsDialog.checkBoxRemeberRecentImages->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxImageFitToWindow->setChecked(settings->value(m_uiSettingsDialog.checkBoxImageFitToWindow->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxImageDrawBorder->setChecked(settings->value(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis()).toBool());
    m_uiSettingsDialog.toolButtonBorderColor->setEnabled(settings->value(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis()).toBool());
    m_borderColor = settings->value("image/border/color").value<QColor>();
    m_backgroundColor = settings->value("image/background/color").value<QColor>();
}

void SettingsDialog::onAccept()
{
    std::shared_ptr<QSettings> settings = Settings::userSettings();
    settings->setValue(m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->whatsThis(), m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxWindowHideStatusbar->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideStatusbar->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxWindowHideToolbar->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideToolbar->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxWindowHideNavigation->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideNavigation->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideToolbar->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideToolbar->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideNavigation->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideNavigation->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxRemeberRecentImages->whatsThis(), m_uiSettingsDialog.checkBoxRemeberRecentImages->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxImageFitToWindow->whatsThis(), m_uiSettingsDialog.checkBoxImageFitToWindow->isChecked());
    settings->setValue(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis(), m_uiSettingsDialog.checkBoxImageDrawBorder->isChecked());
    settings->setValue("image/border/color", m_borderColor);
    settings->setValue("image/background/color", m_backgroundColor);

    // store all shortcuts in user settings
    for(int i = 0; i < m_uiSettingsDialog.tableShortcutsWidget->rowCount(); i++)
    {
        QTableWidgetItem *item = m_uiSettingsDialog.tableShortcutsWidget->item(i, 0);

        if (item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            SettingsShortcutsTableWidgetItem *shortcutItem = static_cast<SettingsShortcutsTableWidgetItem*>(item);
            settings->setValue(shortcutItem->action().whatsThis(), shortcutItem->keySequence());
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
    std::shared_ptr<QSettings> settings = Settings::defaultSettings();
    initializeUI(settings);

    //Todo: menus
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
