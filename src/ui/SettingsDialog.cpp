/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "SettingsDialog.h"

#include "../ui/support/Languages.h"
#include "../ui/support/SettingsStrings.h"
#include "support/SettingsShortcutsTableWidgetItem.h"
#include <QColorDialog>
#include <QSignalBlocker>

SettingsDialog::SettingsDialog(std::unique_ptr<QSettings> defaultSettings,
                               std::unique_ptr<QSettings> userSettings,
                               QWidget *parent)
                                        : QDialog(parent), m_defaultSettings(defaultSettings.release()), m_userSettings(userSettings.release())
{
    Q_ASSERT(defaultSettings);
    Q_ASSERT(userSettings);

    m_uiSettingsDialog.setupUi(this);

    {
        QSignalBlocker signalBlocker(m_uiSettingsDialog.comboBoxLanguage);

        // Populate a localization combobox
        for (const auto &record : Languages::m_localizations)
            m_uiSettingsDialog.comboBoxLanguage->addItem(record.m_language, record.m_code);
    }

    initializeUI(m_userSettings.get());
}

void SettingsDialog::populateShortcuts(const QMenu *menu) const
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

        // Skip all action which are not for key re-assignment
        if (action->whatsThis().isEmpty())
            continue;

        const int rowCount = m_uiSettingsDialog.tableShortcutsWidget->rowCount();
        m_uiSettingsDialog.tableShortcutsWidget->insertRow(rowCount);
        auto headerItem = std::make_unique<QTableWidgetItem>(action->toolTip());
        m_uiSettingsDialog.tableShortcutsWidget->setVerticalHeaderItem(rowCount, headerItem.release());

        auto item = std::make_unique<SettingsShortcutsTableWidgetItem>(*action);
        m_uiSettingsDialog.tableShortcutsWidget->setItemAtCoordinates(rowCount, 0, item.release());
    }
}

void SettingsDialog::initializeUI(const QSettings * const settings)
{
    m_uiSettingsDialog.checkBoxUseSystemLanguage->setChecked(settings->value(m_uiSettingsDialog.checkBoxUseSystemLanguage->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->setChecked(settings->value(m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideStatusbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideStatusbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideToolbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideToolbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideNavigation->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideNavigation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxWindowHideInformation->setChecked(settings->value(m_uiSettingsDialog.checkBoxWindowHideInformation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideToolbar->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideToolbar->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideNavigation->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideNavigation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxFullscreenHideInformation->setChecked(settings->value(m_uiSettingsDialog.checkBoxFullscreenHideInformation->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxRememberRecentImages->setChecked(settings->value(m_uiSettingsDialog.checkBoxRememberRecentImages->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxImageFitToWindow->setChecked(settings->value(m_uiSettingsDialog.checkBoxImageFitToWindow->whatsThis()).toBool());
    m_uiSettingsDialog.checkBoxImageDrawBorder->setChecked(settings->value(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis()).toBool());
    m_uiSettingsDialog.toolButtonBorderColor->setEnabled(settings->value(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis()).toBool());
    m_borderColor = settings->value(SETTINGS_IMAGE_BORDER_COLOR).value<QColor>();
    m_backgroundColor = settings->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>();
    m_languageCode = settings->value(SETTINGS_LANGUAGE_CODE).value<QString>();

    if (auto findIt = std::ranges::find_if(begin(Languages::m_localizations),
                                           end(Languages::m_localizations),
                                           [&m_languageCode = m_languageCode](const Languages::Record &record){
                                               return m_languageCode == record.m_code;
                                           }); findIt != std::end(Languages::m_localizations))
    {
        const auto position = std::distance(Languages::m_localizations.begin(), findIt);
        m_uiSettingsDialog.comboBoxLanguage->setCurrentIndex(position);
    }
}

void SettingsDialog::onAccept()
{
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxUseSystemLanguage->whatsThis(), m_uiSettingsDialog.checkBoxUseSystemLanguage->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->whatsThis(), m_uiSettingsDialog.checkBoxGeneralStartInFullscreen->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxWindowHideStatusbar->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideStatusbar->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxWindowHideToolbar->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideToolbar->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxWindowHideNavigation->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideNavigation->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxWindowHideInformation->whatsThis(), m_uiSettingsDialog.checkBoxWindowHideInformation->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideStatusbar->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideToolbar->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideToolbar->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideNavigation->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideNavigation->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxFullscreenHideInformation->whatsThis(), m_uiSettingsDialog.checkBoxFullscreenHideInformation->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxRememberRecentImages->whatsThis(), m_uiSettingsDialog.checkBoxRememberRecentImages->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxImageFitToWindow->whatsThis(), m_uiSettingsDialog.checkBoxImageFitToWindow->isChecked());
    m_userSettings->setValue(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis(), m_uiSettingsDialog.checkBoxImageDrawBorder->isChecked());
    m_userSettings->setValue(SETTINGS_IMAGE_BORDER_COLOR, m_borderColor);
    m_userSettings->setValue(SETTINGS_IMAGE_BACKGROUND_COLOR, m_backgroundColor);
    m_userSettings->setValue(SETTINGS_LANGUAGE_CODE, m_languageCode);

    // store all shortcuts in user settings
    for (int i = 0; i < m_uiSettingsDialog.tableShortcutsWidget->rowCount(); ++i)
    {
        if (QTableWidgetItem *item = m_uiSettingsDialog.tableShortcutsWidget->item(i, 0); item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            if (const auto * const shortcutItem = dynamic_cast<SettingsShortcutsTableWidgetItem *>(item))
                m_userSettings->setValue(shortcutItem->action().whatsThis(), shortcutItem->keySequence());
        }
    }

    QDialog::accept();
}

void SettingsDialog::onButtonBoxButtonClicked(QAbstractButton *button)
{
    if (QDialogButtonBox::ButtonRole::ResetRole == m_uiSettingsDialog.buttonBox->buttonRole(button))
        onRestoreDefaultsTriggered();
}

void SettingsDialog::onLanguageChanged(int index)
{
    qDebug() << "Selected localization: " << Languages::m_localizations[index].m_language;
    m_languageCode = Languages::m_localizations[index].m_code;
}

void SettingsDialog::onRejected()
{
    // restore all shortcuts from user settings
    for (int i = 0; i < m_uiSettingsDialog.tableShortcutsWidget->rowCount(); ++i)
    {
        if (auto * const item = m_uiSettingsDialog.tableShortcutsWidget->item(i, 0); item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            if (const auto * const shortcutItem = dynamic_cast<SettingsShortcutsTableWidgetItem *>(item))
                shortcutItem->action().setShortcut(m_userSettings->value(shortcutItem->action().whatsThis()).value<QKeySequence>());
        }
    }

    QDialog::reject();
}

void SettingsDialog::onRestoreDefaultsTriggered()
{
    initializeUI(m_defaultSettings.get());

    // restore all shortcuts from default settings
    for (int i = 0; i < m_uiSettingsDialog.tableShortcutsWidget->rowCount(); ++i)
    {
        if (auto * const item = m_uiSettingsDialog.tableShortcutsWidget->item(i, 0); item->type() == SettingsShortcutsTableWidgetItem::type)
        {
            if (auto * const shortcutItem = dynamic_cast<SettingsShortcutsTableWidgetItem *>(item))
                shortcutItem->onKeySequenceChanged(m_defaultSettings->value(shortcutItem->action().whatsThis()).value<QKeySequence>());
        }
    }

    // re-draw the restored shortcuts
    m_uiSettingsDialog.tableShortcutsWidget->updateShortcuts();
}

void SettingsDialog::onToolButtonBorderColorClicked()
{
    if (QColor borderColor { QColorDialog::getColor(m_borderColor) }; borderColor.isValid())
        m_borderColor = borderColor;
}

void SettingsDialog::onToolButtonBackgroundColorClicked()
{
    if (QColor backgroundColor { QColorDialog::getColor(m_backgroundColor) }; backgroundColor.isValid())
        m_backgroundColor = backgroundColor;
}
