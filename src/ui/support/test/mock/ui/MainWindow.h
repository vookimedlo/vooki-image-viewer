/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE
****************************************************************************/


#include <QMainWindow>
#include <array>
#include "../QSettingsMock.h"
#include "../SettingsMock.h"
#include "ui_MainWindow.h"


class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        m_ui.setupUi(this);
    };

    std::array<const QMenu *, 4> allMenus() const { return {m_ui.menuFile, m_ui.menuView, m_ui.menuWindow, m_ui.menuHelp}; };

public slots:
    void onAboutToQuit() const {};
    void onOpenFileRequested([[maybe_unused]] const QString &path) const {};

private slots:
    void onAboutTriggered() const {};
    void onAboutComponentsTriggered() const {};
    void onAboutQtTriggered() const {};
    void onAboutSupportedImageFormats() const {};
    void onClearHistory() const {};
    void onDocsDirClicked() const {};
    void onFileSystemTreeViewActivated([[maybe_unused]] const QModelIndex &index) const {};
    void onFitToWindowToggled([[maybe_unused]] bool toggled) const {};
    void onFullScreenToggled([[maybe_unused]] bool toggled) const {};
    void onHomeDirClicked() const {};
    void onImageDimensionsChanged([[maybe_unused]] int width, [[maybe_unused]] int height) const {};
    void onImageSizeChanged([[maybe_unused]] uint64_t size) const {};
    void onNextImageTriggered() const {};
    void onOriginalSizeTriggered() const {};
    void onPicturesDirClicked() const {};
    void onPreviousImageTriggered() const {};
    void onQuitTriggered() const {};
    void onRecentFileTriggered([[maybe_unused]] const QString &filePath) const {};
    void onReleaseNotesTriggered() const {};
    void onSettingsTriggered() const {};
    void onStatusBarToggled([[maybe_unused]] bool toggled) const {};
    void onZoomInTriggered() const {};
    void onZoomOutTriggered() const {};
    void onZoomPercentageChanged([[maybe_unused]] qreal value) const {};

private:
    Ui::MainWindow m_ui;
};
