#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
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
    [[nodiscard]] QString getRecentFile(int item) const;
    static void loadTranslators();
    void propagateBorderSettings() const;
    QString registerProcessedImage(const QString &filePath, bool addToRecentFiles = true);
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
    Ui::MainWindow m_ui;
    QFileSystemModel *m_fileSystemModel;
    FileSystemSortFilterProxyModel *m_sortFileSystemModel;
    ImageCatalog m_catalog;

    struct
    {
        bool isFileSystemNavigationVisible;
        bool isStatusBarVisible;
        bool isToolBarVisible;
    } m_widgetVisibilityPriorFullscreen;
};
