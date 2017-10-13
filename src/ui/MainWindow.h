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
    ~MainWindow();
    DISABLE_COPY_MOVE(MainWindow);

    enum HANDLE_RESULT_E
    {
        HANDLE_RESULT_E_OK,
        HANDLE_RESULT_E_NOT_READABLE,
        HANDLE_RESULT_E_DONT_EXIST,
        HANDLE_GENERAL_ERROR
    };

    HANDLE_RESULT_E handleImagePath(const QString &path, const bool addToRecentFiles = true);

protected:
    void propagateBorderSettings() const;
    QString getRecentFile(const int item) const;
    QString registerProcessedImage(const QString &filePath, const bool addToRecentFiles = true);
    void restoreRecentFiles();
    void showImage(bool addToRecentFiles);

public slots:
    void onAboutToQuit() const;
    void onOpenFileRequested(const QString &path);

protected slots:
    void onQuitTriggered();
    void onZoomInTriggered() const;
    void onZoomOutTriggered() const;
    void onFitToWindowToggled(const bool toggled) const;
    void onFullScreenToggled(const bool toggled);
    void onFileSystemTreeViewActivated(const QModelIndex &index);
    void onStatusBarToggled(const bool toggled) const;
    void onOriginalSizeTriggered() const;
    void onPreviousImageTriggered();
    void onNextImageTriggered();
    void onAboutTriggered();
    void onAboutComponentsTriggered();
    void onAboutQtTriggered();
    void onRecentFileTriggered(const QString &filePath);
    void onClearHistory() const;
    void onAboutSupportedImageFormats();
    void onSettingsTriggered();
    void onZoomPercentageChanged(const qreal value) const;

private:
    Ui::MainWindow m_ui;
    QFileSystemModel *m_fileSystemModel;
    FileSystemSortFilterProxyModel *m_sortFileSystemModel;
    ImageCatalog m_catalog;

    struct
    {
        bool isStatusBarVisible;
        bool isToolBarVisible;
        bool isFileSystemNavigationVisible;
    } m_widgetVisibilityPriorFullscreen;
};
