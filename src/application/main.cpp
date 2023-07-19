/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../abstraction/init.h"
#include "../ui/MainWindow.h"
#include "Application.h"
#include <iostream>

int main(int argc, char *argv[])
{
    constexpr int firstPassedArg {2};
    const char *requestedPath = nullptr;

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
