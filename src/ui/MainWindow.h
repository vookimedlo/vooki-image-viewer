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

#include <QString>
#include "ui_MainWindow.h"
#include "../model/ImageCatalog.h"
#include "../util/compiler.h"

//Forward declarations
class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    DISABLE_COPY_MOVE(MainWindow);

protected:
    QString registerProcessedImage(const QString& filePath, bool addToRecentFiles = true);
    void showImage(const QString &filePath, bool addToRecentFiles = true);

private slots:
    void onQuitTriggered();
    void onZoomInTriggered();
    void onZoomOutTriggered();
    void onFitToWindowToggled(bool toggled);
    void onFullScreenToggled(bool toggled);
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onStatusBarToggled(bool toggled);
    void onOriginalSizeTriggered();
    void onPreviousImageTriggered();
    void onNextImageTriggered();
    void onAboutTriggered();
    void onAboutComponentsTriggered();
    void onAboutQtTriggered();
    void onFlipHorizontallyTriggered();
    void onFlipVerticallyTriggered();
    void onRotateLeftTriggered();
    void onRotateRightTriggered();
    void onRecentFileTriggered(const QString &filePath);
    void onClearHistory();

private:
    Ui::MainWindow m_ui;
    QFileSystemModel *m_fileSystemModel;
    ImageCatalog m_catalog;

    struct
    {
        bool isStatusBarVisible;
        bool isToolBarVisible;
        bool isFileSystemNavigationVisible;
    } m_widgetVisibilityPriorFullscreen;
};

