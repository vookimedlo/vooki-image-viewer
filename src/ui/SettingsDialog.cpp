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
#include "../util/misc.h"

SettingsDialog::SettingsDialog(std::unique_ptr<QSettings> defaultSettings,
                               std::unique_ptr<QSettings> userSettings,
                               QWidget *parent)
                                        : QDialog(parent),
                                        m_defaultSettings(defaultSettings.release()),
                                        m_userSettings(userSettings.release()),
                                        m_settingsCheckboxes {
                                            &m_uiSettingsDialog.checkBoxUseSystemLanguage,
                                            &m_uiSettingsDialog.checkBoxGeneralStartInFullscreen,
                                            &m_uiSettingsDialog.checkBoxWindowHideStatusbar,
                                            &m_uiSettingsDialog.checkBoxWindowHideToolbar,
                                            &m_uiSettingsDialog.checkBoxWindowHideNavigation,
                                            &m_uiSettingsDialog.checkBoxWindowHideInformation,
                                            &m_uiSettingsDialog.checkBoxFullscreenHideStatusbar,
                                            &m_uiSettingsDialog.checkBoxFullscreenHideToolbar,
                                            &m_uiSettingsDialog.checkBoxFullscreenHideNavigation,
                                            &m_uiSettingsDialog.checkBoxFullscreenHideInformation,
                                            &m_uiSettingsDialog.checkBoxRememberRecentImages,
                                            &m_uiSettingsDialog.checkBoxImageFitToWindow,
                                            &m_uiSettingsDialog.checkBoxImageDrawBorder,
                                        }
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
    Q_ASSERT(menu);

    const auto allActions = Util::getAllActionsHavingShortcut(menu);

    for (const auto action : allActions)
    {
        const int rowCount = m_uiSettingsDialog.tableShortcutsWidget->rowCount();
        m_uiSettingsDialog.tableShortcutsWidget->insertRow(rowCount);
        auto headerItem = std::make_unique<QTableWidgetItem>(action->toolTip());
        m_uiSettingsDialog.tableShortcutsWidget->setVerticalHeaderItem(rowCount, headerItem.release());

        auto item = std::make_unique<SettingsShortcutsTableWidgetItem>(*const_cast<QAction *>(action));
        m_uiSettingsDialog.tableShortcutsWidget->setItemAtCoordinates(rowCount, 0, item.release());
    }
}

QColor SettingsDialog::getColorFromPicker(const QColor &initialColor) const
{
    return QColorDialog::getColor(initialColor);
}

void SettingsDialog::initializeUI(const QSettings * const settings)
{
    Q_ASSERT(settings);

    for (const auto checkbox : m_settingsCheckboxes)
        (*checkbox)->setChecked(settings->value((*checkbox)->whatsThis()).toBool());

    m_uiSettingsDialog.toolButtonBorderColor->setEnabled(settings->value(m_uiSettingsDialog.checkBoxImageDrawBorder->whatsThis()).toBool());
    m_borderColor = settings->value(SETTINGS_IMAGE_BORDER_COLOR).value<QColor>();
    m_backgroundColor = settings->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>();
    m_languageCode = settings->value(SETTINGS_LANGUAGE_CODE).value<QString>();

    if (std::input_iterator auto findIt = std::ranges::find_if(begin(Languages::m_localizations),
                                           end(Languages::m_localizations),
                                           [&languageCode = m_languageCode](const Languages::Record &record){
                                               return languageCode == record.m_code;
                                           }); findIt != std::end(Languages::m_localizations))
    {
        const auto position = std::distance(Languages::m_localizations.begin(), findIt);
        m_uiSettingsDialog.comboBoxLanguage->setCurrentIndex(position);
    }
}

void SettingsDialog::onAccept()
{
    for (const auto checkbox : m_settingsCheckboxes)
        m_userSettings->setValue((*checkbox)->whatsThis(), (*checkbox)->isChecked());

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
    Q_ASSERT(button);

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
    if (const QColor borderColor { getColorFromPicker(m_borderColor) }; borderColor.isValid())
        m_borderColor = borderColor;
}

void SettingsDialog::onToolButtonBackgroundColorClicked()
{
    if (const QColor backgroundColor { getColorFromPicker(m_backgroundColor) }; backgroundColor.isValid())
        m_backgroundColor = backgroundColor;
}
