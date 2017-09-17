#pragma once

#include "ui_MainWindow.h"

#include "../model/ImageCatalog.h"

//Forward declarations
class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onQuitTriggered();
    void onZoomInTriggered();
    void onZoomOutTriggered();
    void onFitToWindowToggled(bool toggled);
    void onFullScreenTriggered();
    void onTreeViewDoubleClicked(const QModelIndex &index);
    void onStatusBarToggled(bool toggled);
    void onOriginalSizeTriggered();
    void onPreviousImageTriggered();
    void onNextImageTriggered();

private:
    Ui::MainWindow ui;
    QFileSystemModel *fileSystemModel;
    ImageCatalog m_catalog;
};

