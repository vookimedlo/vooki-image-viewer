#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtCore>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class QFileSystemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Quit_triggered();

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionFi_t_to_Window_toggled(bool arg1);

    void on_action_FullScreen_triggered();
private:
    Ui::MainWindow *ui;
    QFileSystemModel *fileSystemModel;
};

#endif // MAINWINDOW_H
