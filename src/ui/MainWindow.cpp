#include "MainWindow.h"
#include "ui_MainWindow.h"


#include <QFileSystemModel>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    fileSystemModel(new QFileSystemModel())
{
    ui->setupUi(this);

    fileSystemModel->setRootPath(QDir::currentPath());
    ui->treeView->setModel(fileSystemModel);
    for(int i = 1; i < fileSystemModel->columnCount(); i++)
        ui->treeView->setColumnHidden(i, true);

    QStringList filters;
    filters << "*.JPG";
    filters << "*.BMP";
    filters << "*.GIF";
    filters << "*.JPEG";
    filters << "*.PNG";
    filters << "*.PBM";
    filters << "*.PGM";
    filters << "*.PPM";
    filters << "*.XBM";
    filters << "*.XPM";

    fileSystemModel->setNameFilters(filters);
    fileSystemModel->setNameFilterDisables(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Quit_triggered()
{
    close();
}

void MainWindow::on_action_FullScreen_triggered()
{
    if(isFullScreen())
    {
        showNormal();
        ui->toolBar->show();
        ui->mainToolBar->show();
        ui->dockWidget->show();
        ui->statusBar->show();
    }
    else
    {
        ui->toolBar->hide();
        ui->mainToolBar->hide();
        ui->dockWidget->hide();
        ui->statusBar->hide();
        showFullScreen();
    }
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString filePath = fileSystemModel->filePath(index);
    ui->imageAreaWidget->showImage(filePath);
}

void MainWindow::on_actionZoom_In_triggered()
{
    ui->imageAreaWidget->zoomImageIn(0.10);
}

void MainWindow::on_actionZoom_Out_triggered()
{
    ui->imageAreaWidget->zoomImageOut(0.10);
}

void MainWindow::on_actionFi_t_to_Window_toggled(bool checked)
{
    ui->imageAreaWidget->setFitToWindow(checked);
}
