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

#include "MainWindow.h"

#include "../model/FileSystemSortFilterProxyModel.h"
#include "../ui/support/Settings.h"
#include "../ui/support/SettingsStrings.h"
#include "../util/misc.h"
#include "AboutComponentsDialog.h"
#include "ReleaseNotesDialog.h"
#include "version.h"
#if defined __APPLE__
#include "kdmactouchbar.h"
#endif // __APPLE__
#include "../application/Application.h"
#include "SettingsDialog.h"
#include "support/RecentFileAction.h"
#include "ui_AboutDialog.h"
#include "ui_AboutSupportedFormatsDialog.h"
#include <QAction>
#include <QFileSystemModel>
#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
                                        : QMainWindow(parent)
                                        , m_fileSystemModel(new QFileSystemModel(this))
                                        , m_sortFileSystemModel(new FileSystemSortFilterProxyModel(this))
                                        , m_catalog(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()))
{
    m_ui.setupUi(this);

#if defined __APPLE__
    auto *touchBar = new KDMacTouchBar(this);
    touchBar->setTouchButtonStyle(KDMacTouchBar::IconOnly);
    touchBar->addAction(m_ui.actionZoomIn);
    touchBar->addAction(m_ui.actionZoomOut);
    touchBar->addAction(m_ui.actionOriginalSize);
    touchBar->addAction(m_ui.actionRotateLeft);
    touchBar->addAction(m_ui.actionRotateRight);
    touchBar->addAction(m_ui.actionFlipHorizontally);
    touchBar->addAction(m_ui.actionFlipVertically);
    touchBar->addAction(m_ui.actionPreviousImage);
    touchBar->addAction(m_ui.actionNextImage);
    touchBar->setHidden(true);
#endif // __APPLE__

    m_ui.toolBar->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_T));
    m_ui.toolBar->toggleViewAction()->setWhatsThis("viv/shortcut/window/toolbar");
    m_ui.menuShow->addAction(m_ui.toolBar->toggleViewAction());

    m_ui.dockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_N));
    m_ui.dockWidget->toggleViewAction()->setWhatsThis("viv/shortcut/window/navigation");
    m_ui.menuShow->addAction(m_ui.dockWidget->toggleViewAction());

    m_sortFileSystemModel->setSourceModel(m_fileSystemModel);

    m_fileSystemModel->setRootPath(QDir::currentPath());
    m_ui.fileSystemTreeView->setModel(m_sortFileSystemModel);
    for (int i = 1; i < m_fileSystemModel->columnCount(); i++)
        m_ui.fileSystemTreeView->setColumnHidden(i, true);

    m_ui.fileSystemTreeView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
    m_fileSystemModel->setNameFilters(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()));
    m_fileSystemModel->setNameFilterDisables(false);
    m_fileSystemModel->setFilter(QDir::Filter::Hidden | QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot | QDir::Filter::AllDirs);

    Settings::initializeSettings();
    Settings::initializeSettings(m_ui.menuFile);
    Settings::initializeSettings(m_ui.menuView);
    Settings::initializeSettings(m_ui.menuWindow);
    Settings::initializeSettings(m_ui.menuHelp);

    const std::shared_ptr<QSettings> settings = Settings::userSettings();
    m_ui.toolBar->setHidden(settings->value(SETTINGS_WINDOW_HIDE_TOOLBAR).toBool());
    m_ui.dockWidget->setHidden(settings->value(SETTINGS_WINDOW_HIDE_NAVIGATION).toBool());
    m_ui.toolBar->toggleViewAction()->setChecked(!settings->value(SETTINGS_WINDOW_HIDE_TOOLBAR).toBool());
    m_ui.dockWidget->toggleViewAction()->setChecked(!settings->value(SETTINGS_WINDOW_HIDE_NAVIGATION).toBool());

    if (settings->value(SETTINGS_IMAGE_FITIMAGETOWINDOW).toBool())
        m_ui.actionFitToWindow->setChecked(true);

    if (settings->value(SETTINGS_WINDOW_HIDE_STATUSBAR).toBool())
        m_ui.actionStatusBar->setChecked(false);

    if (settings->value(SETTINGS_GENERAL_FULLSCREEN).toBool())
        m_ui.actionFullScreen->toggle();

    propagateBorderSettings();
    restoreRecentFiles();
    loadTranslators();
}

MainWindow::~MainWindow() = default;

MainWindow::HANDLE_RESULT_E MainWindow::handleImagePath(const QString &path, const bool addToRecentFiles)
{
    if (QFileInfo info(path); info.exists())
    {
        if (info.isReadable())
        {
            if (info.isDir())
            {
                m_catalog.initialize(QDir(path));
                showImage(addToRecentFiles);
                return HANDLE_RESULT_E::OK;
            }
            else if (info.isFile())
            {
                m_catalog.initialize(QFile(path));
                showImage(addToRecentFiles);
                return HANDLE_RESULT_E::OK;
            }
        }

        return HANDLE_RESULT_E::NOT_READABLE;
    }

    return HANDLE_RESULT_E::DONT_EXIST;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event != nullptr)
    {
        switch (event->type())
        {
            // this event is sent if a translator is loaded
            case QEvent::LanguageChange:
                m_ui.retranslateUi(this);
                break;

            // this event is sent, if the system, language changes
            case QEvent::LocaleChange:
                loadTranslators();
                break;
            default:
                break;
        }
    }
    QMainWindow::changeEvent(event);
}

QString MainWindow::getRecentFile(qsizetype item) const
{
    const qsizetype index = item + 1;

    // The first two actions are Clear History & Menu Separator, which are out of our interest
    if (auto actions = m_ui.menuRecentFiles->actions(); 2 < actions.size() && index < actions.size())
    {
        const auto *recentImage = dynamic_cast<RecentFileAction *>(actions.at(index));
        return recentImage->text();
    }

    return {};
}

void MainWindow::loadTranslators()
{
    const std::shared_ptr<QSettings> settings = Settings::userSettings();
    if (auto *application = dynamic_cast<Application *>(QCoreApplication::instance()))
    {
        if (settings->value(SETTINGS_LANGUAGE_USE_SYSTEM).toBool())
            application->installTranslators(QLocale());
        else
            application->installTranslators(QLocale(settings->value(SETTINGS_LANGUAGE_CODE).value<QString>()));
    }
}

void MainWindow::propagateBorderSettings() const
{
    const std::shared_ptr<QSettings> settings = Settings::userSettings();
    m_ui.imageAreaWidget->drawBorder(settings->value(SETTINGS_IMAGE_BORDER_DRAW).toBool(), settings->value(SETTINGS_IMAGE_BORDER_COLOR).value<QColor>());
}

QString MainWindow::registerProcessedImage(const QString &filePath, const bool addToRecentFiles)
{
    if (filePath.isEmpty())
        return {};

    if (addToRecentFiles)
    {
        auto actions = m_ui.menuRecentFiles->actions();

        // Remove all current actions from menu widget
        for (QAction *action : actions)
        {
            m_ui.menuRecentFiles->removeAction(action);
        }

        {
            auto recentImage = std::make_unique<RecentFileAction>(filePath, this);
            recentImage->setStatusTip(filePath);

            QObject::connect(recentImage.get(), &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

            // Add the action just after the separator
            actions.insert(2, recentImage.release());
            m_ui.menuRecentFiles->insertActions(nullptr, actions);
        }

        // Remove the entry exceeding the allowed limit of menu items in recent files menu
        const int maxRecentFiles = 7;
        if (actions.size() > maxRecentFiles)
        {
            std::unique_ptr<RecentFileAction> recentImage(dynamic_cast<RecentFileAction *>(actions.at(maxRecentFiles)));
            QObject::disconnect(recentImage.get(), &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);
            actions.removeAt(maxRecentFiles);
        }
    }

    m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(m_catalog.getCurrent())));
    m_ui.statusBar->showMessage(filePath);
    return filePath;
}

void MainWindow::restoreRecentFiles()
{
    const std::shared_ptr<QSettings> settings = Settings::userSettings();
    if (settings->value(SETTINGS_IMAGE_REMEMBER_RECENT).toBool())
    {
        const std::vector<QString> settingsKeys = {
            SETTINGS_RECENT_FILE_5, SETTINGS_RECENT_FILE_4, SETTINGS_RECENT_FILE_3, SETTINGS_RECENT_FILE_2, SETTINGS_RECENT_FILE_1
        };

        auto actions = m_ui.menuRecentFiles->actions();

        // Remove all current actions from menu widget
        for (QAction *action : actions)
            m_ui.menuRecentFiles->removeAction(action);

        for (const QString &key : settingsKeys)
        {
            QString value = settings->value(key).toString();
            if (value.isEmpty())
                continue;

            auto recentImage = std::make_unique<RecentFileAction>(value, this);
            recentImage->setStatusTip(value);

            QObject::connect(recentImage.get(), &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

            // Add the action just after the separator
            actions.insert(2, recentImage.release());
        }

        m_ui.menuRecentFiles->insertActions(nullptr, actions);
    }
}

void MainWindow::showImage(const bool addToRecentFiles)
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getCurrent(), addToRecentFiles));
}

void MainWindow::onAboutToQuit() const
{
    const std::vector<QString> settingsKeys {
        SETTINGS_RECENT_FILE_1, SETTINGS_RECENT_FILE_2, SETTINGS_RECENT_FILE_3, SETTINGS_RECENT_FILE_4, SETTINGS_RECENT_FILE_5
    };

    std::shared_ptr<QSettings> settings = Settings::userSettings();

    for (size_t i = 0; i < settingsKeys.size(); ++i)
    {
        settings->setValue(settingsKeys[i], (settings->value(SETTINGS_IMAGE_REMEMBER_RECENT).toBool()) ? getRecentFile(i + 1) : QString());
    }
}

void MainWindow::onOpenFileRequested(const QString &path)
{
    handleImagePath(path);
}

void MainWindow::onAboutTriggered()
{
    Ui::aboutDialog uiAbout;
    QDialog dialog(this);
    uiAbout.setupUi(&dialog);
    uiAbout.versionLabel->setText(uiAbout.versionLabel->text().arg(Util::getVersionString()));
    const auto resource = uiAbout.textBrowser->loadResource(QTextDocument::MarkdownResource,
                                                            QUrl("qrc:/text/aboutapp"));
    uiAbout.textBrowser->setMarkdown(resource.toString());
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.exec();
}

void MainWindow::onAboutComponentsTriggered()
{
    AboutComponentsDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.exec();
}

void MainWindow::onAboutQtTriggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onAboutSupportedImageFormats()
{
    Ui::AboutSupportedFormatsDialog uiAbout;
    QDialog dialog(this);
    uiAbout.setupUi(&dialog);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.exec();
}

void MainWindow::onClearHistory() const
{
    auto actions = m_ui.menuRecentFiles->actions();
    // Leave the first two actions intact (Clear History & Menu Separator)
    for (int i = 2; i < actions.size(); i++)
    {
        auto *recentImage = dynamic_cast<RecentFileAction *>(actions.at(i));
        QObject::disconnect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);
        m_ui.menuRecentFiles->removeAction(recentImage);
    }
}

void MainWindow::onFileSystemTreeViewActivated(const QModelIndex &index)
{
    const QString filePath = m_fileSystemModel->filePath(m_sortFileSystemModel->mapToSource(index));
    handleImagePath(filePath);
}

void MainWindow::onFitToWindowToggled(const bool toggled) const
{
    m_ui.imageAreaWidget->onSetFitToWindowTriggered(toggled);
}

void MainWindow::onFullScreenToggled([[maybe_unused]] bool toggled)
{
    // It's better to check the real displayed mode instead of the "toggled" flag.
    if (isFullScreen())
    {
        showNormal();
        m_ui.toolBar->toggleViewAction()->setChecked(m_widgetVisibilityPriorFullscreen.isToolBarVisible);
        if (m_widgetVisibilityPriorFullscreen.isToolBarVisible)
            m_ui.toolBar->show();
        else
            m_ui.toolBar->hide();
        m_ui.dockWidget->toggleViewAction()->setChecked(m_widgetVisibilityPriorFullscreen.isFileSystemNavigationVisible);
        if (m_widgetVisibilityPriorFullscreen.isFileSystemNavigationVisible)
            m_ui.dockWidget->show();
        else
            m_ui.dockWidget->hide();
        m_ui.actionStatusBar->setChecked(m_widgetVisibilityPriorFullscreen.isStatusBarVisible);
        if (m_widgetVisibilityPriorFullscreen.isStatusBarVisible)
            m_ui.statusBar->show();
        else
            m_ui.statusBar->hide();
    }
    else
    {
        m_widgetVisibilityPriorFullscreen.isToolBarVisible = m_ui.toolBar->toggleViewAction()->isChecked();
        m_widgetVisibilityPriorFullscreen.isFileSystemNavigationVisible = m_ui.dockWidget->toggleViewAction()->isChecked();
        m_widgetVisibilityPriorFullscreen.isStatusBarVisible = m_ui.actionStatusBar->isChecked();

        const std::shared_ptr<QSettings> settings = Settings::userSettings();
        if (settings->value(SETTINGS_FULLSCREEN_HIDE_TOOLBAR).toBool())
        {
            m_ui.toolBar->toggleViewAction()->setChecked(false);
            m_ui.toolBar->hide();
        }

        if (settings->value(SETTINGS_FULLSCREEN_HIDE_NAVIGATION).toBool())
        {
            m_ui.dockWidget->toggleViewAction()->setChecked(false);
            m_ui.dockWidget->hide();
        }

        if (settings->value(SETTINGS_FULLSCREEN_HIDE_STATUSBAR).toBool())
        {
            m_ui.actionStatusBar->setChecked(false);
            m_ui.statusBar->hide();
        }

        showFullScreen();
    }
}

void MainWindow::onNextImageTriggered()
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getNext()));
}

void MainWindow::onOriginalSizeTriggered() const
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->onZoomResetTriggered();
}

void MainWindow::onPreviousImageTriggered()
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getPrevious()));
}

void MainWindow::onQuitTriggered()
{
    close();
}

void MainWindow::onRecentFileTriggered(const QString &filePath)
{
    handleImagePath(filePath, false);
}

void MainWindow::onReleaseNotesTriggered()
{
    ReleaseNotesDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint);
    dialog.exec();
}

void MainWindow::onSettingsTriggered()
{
    SettingsDialog dialog(this);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.populateShortcuts(m_ui.menuFile);
    dialog.populateShortcuts(m_ui.menuView);
    dialog.populateShortcuts(m_ui.menuWindow);
    dialog.populateShortcuts(m_ui.menuHelp);
    if (dialog.exec() == QDialog::Accepted)
    {
        propagateBorderSettings();
        m_ui.imageAreaWidget->repaintWithTransformations();
        loadTranslators();
    }
}

void MainWindow::onStatusBarToggled(const bool toggled) const
{
    toggled ? m_ui.statusBar->show() : m_ui.statusBar->hide();
}

void MainWindow::onZoomInTriggered() const
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->onZoomImageInTriggered(0.10);
}

void MainWindow::onZoomOutTriggered() const
{
    m_ui.actionFitToWindow->setChecked(false);
    m_ui.imageAreaWidget->onZoomImageOutTriggered(0.10);
}

void MainWindow::onZoomPercentageChanged(const qreal value) const
{
    //: Used in the statusbar showing the zooming percentage. Example: "Zoom: 12%"
    m_ui.statusBar->rightLabel().setText(tr("Zoom: %1%").arg(QString::number(static_cast<int>(value * 100))));
}

void MainWindow::onHomeDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(QDir::homePath())));
}

void MainWindow::onDocsDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    auto locations = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation);
    if (!locations.isEmpty())
        m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(locations.first())));
}

void MainWindow::onPicturesDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    auto locations = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::PicturesLocation);
    if (!locations.isEmpty())
        m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(locations.first())));
}
