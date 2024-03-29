/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "Application.h"
#include <QDebug>
#include <QFileOpenEvent>
#include <QLibraryInfo>

Application::Application(int &argc, char **argv)
                                        : QApplication(argc, argv)
{
    // QLocale::setDefault(QLocale::Language::German);
    //  Localization support
    qDebug() << "QLocale: " << QLocale().name();

    // This is a workaround to get all macOS specific menu items into the translatable state.
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Services");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Hide %1");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Hide Others");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Show %1");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Show All");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "About %1");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Quit %1");
    QCoreApplication::translate("MAC_APPLICATION_MENU", "Preferences...");

    // This is a workaround to get all required specific QDialogButtonBox items into the translatable state.
    QCoreApplication::translate("QPlatformTheme", "OK");
    QCoreApplication::translate("QPlatformTheme", "Cancel");
    QCoreApplication::translate("QPlatformTheme", "Restore Defaults");

    // This is a workaround to get all required specific QFileSystemModel items into the translatable state.
    QCoreApplication::translate("QFileSystemModel", "Name");
}

void Application::installTranslators(const QLocale &locale)
{
    removeTranslator(&qtTranslator);
    removeTranslator(&translator);

    if (qtTranslator.load(locale, "qt", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        installTranslator(&qtTranslator);

    if (translator.load(locale, "VookiImageViewer", "_", ":/i18n"))
        installTranslator(&translator);
}

bool Application::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen)
    {
        const auto openEvent = dynamic_cast<QFileOpenEvent *>(event);
        qDebug() << "Open file " << openEvent->file();
        emit openFileRequested(openEvent->file());
    }

    return QApplication::event(event);
}
