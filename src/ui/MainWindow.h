#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../model/ImageCatalog.h"
#include "../util/compiler.h"
#include "ui_MainWindow.h"
#include <QString>

// Forward declarations
class QFileSystemModel;
class FileSystemSortFilterProxyModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    DISABLE_COPY_MOVE(MainWindow);

    enum class HANDLE_RESULT_E
    {
        OK,
        NOT_READABLE,
        DONT_EXIST,
        GENERAL_ERROR
    };

    HANDLE_RESULT_E handleImagePath(const QString &path, bool addToRecentFiles = true);

protected:
    void changeEvent(QEvent *) override;
    [[nodiscard]] QString getRecentFile(qsizetype item) const;
    static void loadTranslators();
    void propagateBackgroundSettings() const;
    void propagateBorderSettings() const;
    [[nodiscard]] QString registerProcessedImage(const QString &filePath, bool addToRecentFiles = true);
    void restoreRecentFiles();
    void showImage(bool addToRecentFiles);

public slots:
    void onAboutToQuit() const;
    void onOpenFileRequested(const QString &path);

protected slots:
    void onAboutTriggered();
    void onAboutComponentsTriggered();
    void onAboutQtTriggered();
    void onAboutSupportedImageFormats();
    void onClearHistory() const;
    void onDocsDirClicked() const;
    void onFileSystemTreeViewActivated(const QModelIndex &index);
    void onFitToWindowToggled(bool toggled) const;
    void onFullScreenToggled(bool toggled);
    void onHomeDirClicked() const;
    void onImageDimensionsChanged(int width, int height) const;
    void onImageSizeChanged(uint64_t size) const;
    void onNextImageTriggered();
    void onOriginalSizeTriggered() const;
    void onPicturesDirClicked() const;
    void onPreviousImageTriggered();
    void onQuitTriggered();
    void onRecentFileTriggered(const QString &filePath);
    void onReleaseNotesTriggered();
    void onSettingsTriggered();
    void onStatusBarToggled(bool toggled) const;
    void onZoomInTriggered() const;
    void onZoomOutTriggered() const;
    void onZoomPercentageChanged(qreal value) const;

private:
    Ui::MainWindow m_ui {};
    QFileSystemModel *m_fileSystemModel;
    FileSystemSortFilterProxyModel *m_sortFileSystemModel;
    ImageCatalog m_catalog;

    struct
    {
        bool isFileSystemNavigationVisible;
        bool isStatusBarVisible;
        bool isToolBarVisible;
        bool isInformationVisible;
    } m_widgetVisibilityPriorFullscreen {};
};
