#include <QApplication>
#include "abstraction/init.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SystemDependant::Init();
    MainWindow w;
    w.show();

    return a.exec();
}
