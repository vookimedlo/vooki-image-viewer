#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include "ui_SettingsDialog.h"
#include <QColor>
#include <QDialog>
#include <QMenu>
#include <QSettings>
#include <memory>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(std::unique_ptr<QSettings> defaultSettings,
                            std::unique_ptr<QSettings> userSettings,
                            QWidget *parent = nullptr);
    DISABLE_COPY_MOVE(SettingsDialog);

    void populateShortcuts(const QMenu *menu) const;

protected:
    void initializeUI(const QSettings * settings);

public slots:
    virtual void onAccept();
    virtual void onButtonBoxButtonClicked(QAbstractButton *button);
    virtual void onLanguageChanged(int index);
    virtual void onRejected();
    virtual void onRestoreDefaultsTriggered();
    virtual void onToolButtonBorderColorClicked();
    virtual void onToolButtonBackgroundColorClicked();

protected:
    const Ui::SettingsDialog *ui() const { return &m_uiSettingsDialog; };

private:
    QColor m_borderColor;
    QColor m_backgroundColor;
    QString m_languageCode;
    std::unique_ptr<QSettings> m_defaultSettings;
    std::unique_ptr<QSettings> m_userSettings;
    Ui::SettingsDialog m_uiSettingsDialog;
};
