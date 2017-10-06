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

#include <iostream>
#include <QApplication>
#include "abstraction/init.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    const int firstPassedArg = 2;
    char *requestedPath = nullptr;

    switch (argc)
    {
    case 0:
    case 1:
        /* Pass through - no args were passed. */
        break;
    case firstPassedArg:
        requestedPath = argv[firstPassedArg - 1];
        break;
    default:
        std::cerr << "Usage:" << std::endl << argv[0] << " [path_to_file|path_to_dir]" << std::endl;
        return 0;
    }

    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Michal Duda");
    QCoreApplication::setOrganizationDomain("VookiImageViewer.com");
    QCoreApplication::setApplicationName("VookiImageViewer");

    SystemDependant::Init();
    MainWindow w;
    QObject::connect(&a, &QApplication::aboutToQuit, &w, &MainWindow::onAboutToQuit);
    w.handleImagePath(requestedPath);
    w.show();

    return a.exec();
}
