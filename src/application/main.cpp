/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include "../abstraction/init.h"
#include "../ui/MainWindow.h"
#include "Application.h"
#include <iostream>

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

    Application application(argc, argv);
    QCoreApplication::setOrganizationName("Michal Duda");
    QCoreApplication::setOrganizationDomain("VookiImageViewer.cz");
    QCoreApplication::setApplicationName("VookiImageViewer");

#ifdef UNIX_LIKE
    // Unix-like systems shall have our plugins located in the one of the following locations + /imageformats
    QCoreApplication::addLibraryPath("/usr/lib/vookiimageviewer");
    QCoreApplication::addLibraryPath("/usr/local/lib/vookiimageviewer");
    QCoreApplication::addLibraryPath("/usr/lib64/vookiimageviewer");
    QCoreApplication::addLibraryPath("/usr/local/lib64/vookiimageviewer");
#endif // UNIX_LIKE

    SystemDependant::Init();
    MainWindow mainWindow;
    QObject::connect(&application, &Application::aboutToQuit, &mainWindow, &MainWindow::onAboutToQuit);
    QObject::connect(&application, &Application::openFileRequested, &mainWindow, &MainWindow::onOpenFileRequested);
    mainWindow.handleImagePath(requestedPath);
    mainWindow.show();

    return Application::exec();
}
