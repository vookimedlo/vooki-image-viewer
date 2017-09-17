#include "MainWindow.h"

#include <QFileSystemModel>
#include <QImageReader>
#include <QPainter>
#include "../util/misc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    fileSystemModel(new QFileSystemModel()),
    m_catalog(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()))
{
    ui.setupUi(this);

    fileSystemModel->setRootPath(QDir::currentPath());
    ui.treeView->setModel(fileSystemModel);
    for(int i = 1; i < fileSystemModel->columnCount(); i++)
        ui.treeView->setColumnHidden(i, true);

    fileSystemModel->setNameFilters(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()));
    fileSystemModel->setNameFilterDisables(false);
}

MainWindow::~MainWindow()
{

}

void MainWindow::onQuitTriggered()
{
    close();
}

void MainWindow::onFullScreenTriggered()
{
    if(isFullScreen())
    {
        showNormal();
        ui.toolBar->show();
        ui.mainToolBar->show();
        ui.dockWidget->show();
        ui.actionStatusBar->isChecked() ? ui.statusBar->show() : ui.statusBar->hide();
    }
    else
    {
        ui.toolBar->hide();
        ui.mainToolBar->hide();
        ui.dockWidget->hide();
        ui.statusBar->hide();
        showFullScreen();
    }
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    QString filePath = fileSystemModel->filePath(index);
    m_catalog.initialize(QFile(filePath));
    ui.imageAreaWidget->showImage(m_catalog.getCurrent());
}

void MainWindow::onZoomInTriggered()
{
    ui.imageAreaWidget->zoomImageIn(0.10);
}

void MainWindow::onZoomOutTriggered()
{
    ui.imageAreaWidget->zoomImageOut(0.10);
}

void MainWindow::onFitToWindowToggled(bool toggled)
{
    ui.imageAreaWidget->setFitToWindow(toggled);
}

void MainWindow::onStatusBarToggled(bool toggled)
{
   if (isFullScreen())
   {
        ui.statusBar->isHidden() ? ui.statusBar->show() : ui.statusBar->hide();
        // Preserve the previous state once in full-screen mode
        ui.actionStatusBar->blockSignals(true);
        ui.actionStatusBar->setChecked(!toggled);
        ui.actionStatusBar->blockSignals(false);
   }
   else
   {
       toggled ? ui.statusBar->show() : ui.statusBar->hide();
   }
}

void MainWindow::onOriginalSizeTriggered()
{
    ui.imageAreaWidget->zoomReset();
}

void MainWindow::onPreviousImageTriggered()
{
    ui.imageAreaWidget->showImage(m_catalog.getPrevious());
}

void MainWindow::onNextImageTriggered()
{
    ui.imageAreaWidget->showImage(m_catalog.getNext());
}
