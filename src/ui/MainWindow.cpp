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

#include "MainWindow.h"

#include <QDialog>
#include <QFileSystemModel>
#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include "ui_About.h"
#include "AboutComponentsDialog.h"
#include "support/RecentFileAction.h"
#include "../util/compiler.h"
#include "../util/misc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_fileSystemModel(new QFileSystemModel(this)),
    m_catalog(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()))
{
    m_ui.setupUi(this);

    m_ui.toolBar->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_T));
    m_ui.menuShow->addAction(m_ui.toolBar->toggleViewAction());

    m_ui.dockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_N));
    m_ui.menuShow->addAction(m_ui.dockWidget->toggleViewAction());

    m_fileSystemModel->setRootPath(QDir::currentPath());
    m_ui.treeView->setModel(m_fileSystemModel);
    for(int i = 1; i < m_fileSystemModel->columnCount(); i++)
        m_ui.treeView->setColumnHidden(i, true);

    m_fileSystemModel->setNameFilters(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()));
    m_fileSystemModel->setNameFilterDisables(false);
    m_fileSystemModel->setFilter(QDir::Filter::Hidden|QDir::Filter::AllEntries|QDir::Filter::NoDotAndDotDot|QDir::Filter::AllDirs);
}

MainWindow::~MainWindow()
{

}

void MainWindow::onQuitTriggered()
{
    close();
}

void MainWindow::onFullScreenToggled(bool toggled)
{
    UNUSED_VARIABLE(toggled);

    // It's better to check the real displayed mode instead of the "toggled" flag.
    if(isFullScreen())
    {
        showNormal();
        m_ui.toolBar->toggleViewAction()->isChecked() ? m_ui.toolBar->show() : m_ui.toolBar->hide();
        m_ui.dockWidget->toggleViewAction()->isChecked() ? m_ui.dockWidget->show() : m_ui.dockWidget->hide();
        m_ui.actionStatusBar->isChecked() ? m_ui.statusBar->show() : m_ui.statusBar->hide();
    }
    else
    {
        m_ui.toolBar->hide();
        m_ui.dockWidget->hide();
        m_ui.statusBar->hide();
        showFullScreen();
    }
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    const QString filePath = m_fileSystemModel->filePath(index);
    showImage(filePath);
}

void MainWindow::onZoomInTriggered()
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->zoomImageIn(0.10);
}

void MainWindow::onZoomOutTriggered()
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->zoomImageOut(0.10);
}

void MainWindow::onFitToWindowToggled(bool toggled)
{
    m_ui.imageAreaWidget->setFitToWindow(toggled);
}

void MainWindow::onStatusBarToggled(bool toggled)
{
   if (isFullScreen())
   {
        m_ui.statusBar->isHidden() ? m_ui.statusBar->show() : m_ui.statusBar->hide();
        // Preserve the previous state once in full-screen mode
        m_ui.actionStatusBar->blockSignals(true);
        m_ui.actionStatusBar->setChecked(!toggled);
        m_ui.actionStatusBar->blockSignals(false);
   }
   else
   {
       toggled ? m_ui.statusBar->show() : m_ui.statusBar->hide();
   }
}

void MainWindow::onOriginalSizeTriggered()
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->zoomReset();
}

void MainWindow::onPreviousImageTriggered()
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getPrevious()));
}

void MainWindow::onNextImageTriggered()
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getNext()));
}

void MainWindow::onAboutTriggered()
{
    Ui::aboutDialog uiAbout;
    QDialog dialog(this);
    uiAbout.setupUi(&dialog);
    dialog.exec();
}

void MainWindow::onAboutComponentsTriggered()
{
    AboutComponentsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onFlipHorizontallyTriggered()
{
    m_ui.imageAreaWidget->flipHorizontally();
}

void MainWindow::onFlipVerticallyTriggered()
{
    m_ui.imageAreaWidget->flipVertically();
}

void MainWindow::onRotateLeftTriggered()
{
    m_ui.imageAreaWidget->rotateLeft();
}

void MainWindow::onRotateRightTriggered()
{
    m_ui.imageAreaWidget->rotateRight();
}


QString MainWindow::registerProcessedImage(const QString& filePath, bool addToRecentFiles)
{
    if (addToRecentFiles)
    {
        auto actions = m_ui.menuRecentFiles->actions();

        RecentFileAction *recentImage = new RecentFileAction(filePath, this);
        recentImage->setStatusTip(filePath);

        QObject::connect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

        // Add the action just after the separator
        actions.insert(2, recentImage);

        for(QAction *action : actions)
        {
            m_ui.menuRecentFiles->removeAction(action);
        }

        m_ui.menuRecentFiles->insertActions(0, actions);

        // Remove entry exceeding the allowed limit of menu items in recent files menu
        const int maxRecentFiles = 7;
        if (actions.size() > maxRecentFiles)
        {
            RecentFileAction *recentImage = dynamic_cast<RecentFileAction *>(actions.at(maxRecentFiles));
            recentImage->setParent(nullptr);
            QObject::disconnect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);
            delete recentImage;
            actions.removeAt(maxRecentFiles);
        }
    }

    m_ui.statusBar->showMessage(filePath);
    return filePath;
}

void MainWindow::onRecentFileTriggered(const QString &filePath)
{
    showImage(filePath, false);
}

void MainWindow::showImage(const QString &filePath, bool addToRecentFiles)
{
    m_catalog.initialize(QFile(filePath));
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getCurrent(), addToRecentFiles));
}
