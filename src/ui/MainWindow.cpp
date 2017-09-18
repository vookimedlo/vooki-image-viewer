#include "MainWindow.h"

#include <QFileSystemModel>
#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include "../util/misc.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_fileSystemModel(new QFileSystemModel(this)),
    m_catalog(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()))
{
    m_ui.setupUi(this);

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

void MainWindow::onFullScreenTriggered()
{
    if(isFullScreen())
    {
        showNormal();
        m_ui.toolBar->show();
        m_ui.mainToolBar->show();
        m_ui.dockWidget->show();
        m_ui.actionStatusBar->isChecked() ? m_ui.statusBar->show() : m_ui.statusBar->hide();
    }
    else
    {
        m_ui.toolBar->hide();
        m_ui.mainToolBar->hide();
        m_ui.dockWidget->hide();
        m_ui.statusBar->hide();
        showFullScreen();
    }
}

void MainWindow::onTreeViewDoubleClicked(const QModelIndex &index)
{
    QString filePath = m_fileSystemModel->filePath(index);
    m_catalog.initialize(QFile(filePath));
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getCurrent()));
}

void MainWindow::onZoomInTriggered()
{
    m_ui.imageAreaWidget->zoomImageIn(0.10);
}

void MainWindow::onZoomOutTriggered()
{
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

void MainWindow::onAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onFlipHorizontallyTriggered()
{

}

void MainWindow::onFlipVerticallyTriggered()
{

}

void MainWindow::onRotateLeftTriggered()
{
    m_ui.imageAreaWidget->rotateLeft();
}

void MainWindow::onRotateRightTriggered()
{
    m_ui.imageAreaWidget->rotateRight();
}


QString MainWindow::registerProcessedImage(const QString& filepath)
{
    m_ui.statusBar->showMessage(filepath);
    return filepath;
}
