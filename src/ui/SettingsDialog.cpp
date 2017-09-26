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

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
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
    QDialog::accept();
}

void SettingsDialog::onRestoreDefaultsTriggered()
{

}
