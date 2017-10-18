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

#include "MainWindow.h"

#include "../model/FileSystemSortFilterProxyModel.h"
#include "../ui/support/Settings.h"
#include "../ui/support/SettingsStrings.h"
#include "../util/misc.h"
#include "../util/version.h"
#include "AboutComponentsDialog.h"
#include "SettingsDialog.h"
#include "support/RecentFileAction.h"
#include "ui_AboutDialog.h"
#include "ui_AboutSupportedFormatsDialog.h"
#include <QDialog>
#include <QFileSystemModel>
#include <QImageReader>
#include <QMessageBox>
#include <QPainter>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
                                        : QMainWindow(parent)
                                        , m_fileSystemModel(new QFileSystemModel(this))
                                        , m_sortFileSystemModel(new FileSystemSortFilterProxyModel(this))
                                        , m_catalog(Util::convertFormatsToFilters(QImageReader::supportedImageFormats()))
{
    m_ui.setupUi(this);

    m_ui.toolBar->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_T));
    m_ui.toolBar->toggleViewAction()->setWhatsThis("shortcut/window/toolbar");
    m_ui.menuShow->addAction(m_ui.toolBar->toggleViewAction());

    m_ui.dockWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_N));
    m_ui.dockWidget->toggleViewAction()->setWhatsThis("shortcut/window/navigation");
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

    if (settings->value(SETTINGS_WINDOW_HIDE_STATUSBAR).toBool())
        m_ui.actionStatusBar->setChecked(false);

    if (settings->value(SETTINGS_GENERAL_FULLSCREEN).toBool())
        m_ui.actionFullScreen->toggle();

    propagateBorderSettings();
    restoreRecentFiles();
}

MainWindow::~MainWindow() {}

void MainWindow::onQuitTriggered()
{
    close();
}

void MainWindow::onFullScreenToggled(bool toggled)
{
    UNUSED_VARIABLE(toggled);

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

void MainWindow::onFileSystemTreeViewActivated(const QModelIndex &index)
{
    const QString filePath = m_fileSystemModel->filePath(m_sortFileSystemModel->mapToSource(index));
    handleImagePath(filePath);
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

void MainWindow::onFitToWindowToggled(const bool toggled) const
{
    m_ui.imageAreaWidget->onSetFitToWindowTriggered(toggled);
}

void MainWindow::onStatusBarToggled(const bool toggled) const
{
    toggled ? m_ui.statusBar->show() : m_ui.statusBar->hide();
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

void MainWindow::onNextImageTriggered()
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getNext()));
}

void MainWindow::onAboutTriggered()
{
    Ui::aboutDialog uiAbout;
    QDialog dialog(this);
    uiAbout.setupUi(&dialog);
    uiAbout.versionLabel->setText(uiAbout.versionLabel->text().append(Util::getVersionString()));
    dialog.exec();
}

void MainWindow::onAboutComponentsTriggered()
{
    AboutComponentsDialog dialog(this);
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
    dialog.exec();
}

QString MainWindow::registerProcessedImage(const QString &filePath, const bool addToRecentFiles)
{
    if (filePath.isEmpty())
        return QString();

    if (addToRecentFiles)
    {
        auto actions = m_ui.menuRecentFiles->actions();

        // Remove all current actions from menu widget
        for (QAction *action : actions)
        {
            m_ui.menuRecentFiles->removeAction(action);
        }

        RecentFileAction *recentImage = new RecentFileAction(filePath, this);
        recentImage->setStatusTip(filePath);

        QObject::connect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

        // Add the action just after the separator
        actions.insert(2, recentImage);

        m_ui.menuRecentFiles->insertActions(nullptr, actions);

        // Remove entry exceeding the allowed limit of menu items in recent files menu
        const int maxRecentFiles = 7;
        if (actions.size() > maxRecentFiles)
        {
            RecentFileAction *recentImage = dynamic_cast<RecentFileAction *>(actions.at(maxRecentFiles));
            recentImage->setParent(nullptr);
            QObject::disconnect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);
            delete recentImage;
            actions.removeAt(maxRecentFiles);
        }
    }

    m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(m_catalog.getCurrent())));
    m_ui.statusBar->showMessage(filePath);
    return filePath;
}

void MainWindow::onRecentFileTriggered(const QString &filePath)
{
    handleImagePath(filePath, false);
}

void MainWindow::onClearHistory() const
{
    auto actions = m_ui.menuRecentFiles->actions();
    // Leave the first two actions intact (Clear History & Menu Separator)
    for (int i = 2; i < actions.size(); i++)
    {
        RecentFileAction *recentImage = dynamic_cast<RecentFileAction *>(actions.at(i));
        QObject::disconnect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);
        m_ui.menuRecentFiles->removeAction(recentImage);
    }
}

void MainWindow::onSettingsTriggered()
{
    SettingsDialog dialog(this);
    dialog.populateShortcuts(m_ui.menuFile);
    dialog.populateShortcuts(m_ui.menuView);
    dialog.populateShortcuts(m_ui.menuWindow);
    dialog.populateShortcuts(m_ui.menuHelp);
    if (dialog.exec() == QDialog::Accepted)
    {
        propagateBorderSettings();
        m_ui.imageAreaWidget->repaintWithTransformations();
    }
}

void MainWindow::showImage(const bool addToRecentFiles)
{
    m_ui.imageAreaWidget->showImage(registerProcessedImage(m_catalog.getCurrent(), addToRecentFiles));
}

MainWindow::HANDLE_RESULT_E MainWindow::handleImagePath(const QString &path, const bool addToRecentFiles)
{
    QFileInfo info(path);

    if (info.exists())
    {
        if (info.isReadable())
        {
            if (info.isDir())
            {
                m_catalog.initialize(QDir(path));
                showImage(addToRecentFiles);
                return HANDLE_RESULT_E_OK;
            }
            else if (info.isFile())
            {
                m_catalog.initialize(QFile(path));
                showImage(addToRecentFiles);
                return HANDLE_RESULT_E_OK;
            }
        }

        return HANDLE_RESULT_E_NOT_READABLE;
    }

    return HANDLE_RESULT_E_DONT_EXIST;
}

void MainWindow::propagateBorderSettings() const
{
    const std::shared_ptr<QSettings> settings = Settings::userSettings();
    m_ui.imageAreaWidget->drawBorder(settings->value(SETTINGS_IMAGE_BORDER_DRAW).toBool(), settings->value(SETTINGS_IMAGE_BORDER_COLOR).value<QColor>());
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

            RecentFileAction *recentImage = new RecentFileAction(value, this);
            recentImage->setStatusTip(value);

            QObject::connect(recentImage, &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

            // Add the action just after the separator
            actions.insert(2, recentImage);
        }

        m_ui.menuRecentFiles->insertActions(nullptr, actions);
    }
}

QString MainWindow::getRecentFile(const int item) const
{
    const int index = item + 1;
    auto actions = m_ui.menuRecentFiles->actions();

    // The first two actions are Clear History & Menu Separator, which are out of our interest
    if (2 < actions.size() && index < actions.size())
    {
        RecentFileAction *recentImage = dynamic_cast<RecentFileAction *>(actions.at(index));
        return recentImage->text();
    }

    return QString();
}

void MainWindow::onAboutToQuit() const
{
    const std::vector<QString> settingsKeys = {
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

void MainWindow::onZoomPercentageChanged(const qreal value) const
{
    m_ui.statusBar->rightLabel().setText(tr("Zoom: ") + QString::number(static_cast<int>(value * 100)) + "%");
}
