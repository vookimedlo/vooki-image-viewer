/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "MainWindow.h"

#include "../model/FileSystemSortFilterProxyModel.h"
#include "../ui/support/Settings.h"
#include "../ui/support/SettingsStrings.h"
#include "../util//ByteSize.h"
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

#if not QT_CONFIG(whatsthis)
    #error "Qt was not compiled with the whatsthis feature, cannot compile this program which depends on it."
#endif

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

    m_ui.dockInfoWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::Key_E));
    m_ui.dockInfoWidget->toggleViewAction()->setWhatsThis("viv/shortcut/window/info");
    m_ui.menuShow->addAction(m_ui.dockInfoWidget->toggleViewAction());

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
    m_ui.dockInfoWidget->setHidden(settings->value(SETTINGS_WINDOW_HIDE_INFORMATION).toBool());
    m_ui.toolBar->toggleViewAction()->setChecked(!settings->value(SETTINGS_WINDOW_HIDE_TOOLBAR).toBool());
    m_ui.dockWidget->toggleViewAction()->setChecked(!settings->value(SETTINGS_WINDOW_HIDE_NAVIGATION).toBool());
    m_ui.dockInfoWidget->toggleViewAction()->setChecked(!settings->value(SETTINGS_WINDOW_HIDE_INFORMATION).toBool());

    if (settings->value(SETTINGS_IMAGE_FITIMAGETOWINDOW).toBool())
        m_ui.actionFitToWindow->setChecked(true);

    if (settings->value(SETTINGS_WINDOW_HIDE_STATUSBAR).toBool())
        m_ui.actionStatusBar->setChecked(false);

    if (settings->value(SETTINGS_GENERAL_FULLSCREEN).toBool())
        m_ui.actionFullScreen->toggle();

    propagateBackgroundSettings();
    propagateBorderSettings();
    restoreRecentFiles();
    loadTranslators();
}

MainWindow::~MainWindow() = default;

MainWindow::HANDLE_RESULT_E MainWindow::handleImagePath(const QString &path, const bool addToRecentFiles)
{
    m_ui.statusBar->clearLabels();

    if (const QFileInfo info(path); info.exists())
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

QString MainWindow::getRecentFile(const qsizetype item) const
{
    const qsizetype index = item + 1;

    // The first two actions are Clear History & Menu Separator, which are out of our interest
    if (const auto actions = m_ui.menuRecentFiles->actions(); 2 < actions.size() && index < actions.size())
    {
        const auto * const recentImage = dynamic_cast<RecentFileAction *>(actions.at(index));
        return recentImage->text();
    }

    return {};
}

void MainWindow::loadTranslators()
{
    const auto settings = Settings::userSettings();
    if (auto * const application = dynamic_cast<Application *>(QCoreApplication::instance()))
    {
        if (settings->value(SETTINGS_LANGUAGE_USE_SYSTEM).toBool())
            application->installTranslators(QLocale());
        else
            application->installTranslators(QLocale(settings->value(SETTINGS_LANGUAGE_CODE).value<QString>()));
    }
}

void MainWindow::propagateBackgroundSettings() const
{
    const auto settings = Settings::userSettings();
    m_ui.imageAreaWidget->setBackgroundColor(settings->value(SETTINGS_IMAGE_BACKGROUND_COLOR).value<QColor>());
}

void MainWindow::propagateBorderSettings() const
{
    const auto settings = Settings::userSettings();
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
            m_ui.menuRecentFiles->removeAction(action);

        {
            auto recentImage = std::make_unique<RecentFileAction>(filePath, this);
            recentImage->setStatusTip(filePath);

            QObject::connect(recentImage.get(), &RecentFileAction::recentFileActionTriggered, this, &MainWindow::onRecentFileTriggered);

            // Add the action just after the separator
            actions.insert(2, recentImage.release());
            m_ui.menuRecentFiles->insertActions(nullptr, actions);
        }

        // Remove the entry exceeding the allowed limit of menu items in recent files menu
        if (constexpr int maxRecentFiles = 7; actions.size() > maxRecentFiles)
        {
            const std::unique_ptr<RecentFileAction> recentImage(dynamic_cast<RecentFileAction *>(actions.at(maxRecentFiles)));
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
    if (const auto settings = Settings::userSettings(); settings->value(SETTINGS_IMAGE_REMEMBER_RECENT).toBool())
    {
        const std::vector<QString> settingsKeys {
            SETTINGS_RECENT_FILE_5, SETTINGS_RECENT_FILE_4, SETTINGS_RECENT_FILE_3, SETTINGS_RECENT_FILE_2, SETTINGS_RECENT_FILE_1
        };

        auto actions = m_ui.menuRecentFiles->actions();

        // Remove all current actions from menu widget
        for (QAction * const action : actions)
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

    const std::shared_ptr<QSettings> settings = Settings::userSettings();

    for (size_t i = 0; i < settingsKeys.size(); ++i)
        settings->setValue(settingsKeys[i], (settings->value(SETTINGS_IMAGE_REMEMBER_RECENT).toBool()) ? getRecentFile(i + 1) : QString());
}

void MainWindow::onOpenFileRequested(const QString &path)
{
    handleImagePath(path);
}

void MainWindow::onAboutTriggered()
{
    Ui::aboutDialog uiAbout;
    QDialog dialog {this};
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
    const auto actions = m_ui.menuRecentFiles->actions();
    // Leave the first two actions intact (Clear History & Menu Separator)
    for (int i = 2; i < actions.size(); i++)
    {
        auto * const recentImage = dynamic_cast<RecentFileAction *>(actions.at(i));
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
        m_ui.dockInfoWidget->toggleViewAction()->setChecked(m_widgetVisibilityPriorFullscreen.isInformationVisible);
        if (m_widgetVisibilityPriorFullscreen.isInformationVisible)
            m_ui.dockInfoWidget->show();
        else
            m_ui.dockInfoWidget->hide();
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
        m_widgetVisibilityPriorFullscreen.isInformationVisible = m_ui.dockInfoWidget->toggleViewAction()->isChecked();
        m_widgetVisibilityPriorFullscreen.isStatusBarVisible = m_ui.actionStatusBar->isChecked();

        const auto settings = Settings::userSettings();
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

        if (settings->value(SETTINGS_FULLSCREEN_HIDE_INFORMATION).toBool())
        {
            m_ui.dockInfoWidget->toggleViewAction()->setChecked(false);
            m_ui.dockInfoWidget->hide();
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
    ReleaseNotesDialog dialog {this};
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinMaxButtonsHint);
    dialog.exec();
}

void MainWindow::onSettingsTriggered()
{
    SettingsDialog dialog {Settings::defaultSettings(), Settings::userSettings(), this};
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.populateShortcuts(m_ui.menuFile);
    dialog.populateShortcuts(m_ui.menuView);
    dialog.populateShortcuts(m_ui.menuWindow);
    dialog.populateShortcuts(m_ui.menuHelp);
    if (dialog.exec() == QDialog::Accepted)
    {
        propagateBackgroundSettings();
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

void MainWindow::onImageDimensionsChanged(const int width, const int height) const
{
    //: Used in the statusbar showing the image dimensions. Example: "1024x760"
    m_ui.statusBar->setDimensionLabel(tr("%1x%2 px").arg(QString::number(width), QString::number(height)));
}

void MainWindow::onImageSizeChanged(const uint64_t size) const
{
    const ByteSize byteSize {size};
    const auto [newSize, unit] { byteSize.humanReadableSize() };
    const auto unitString { byteSize.getUnit(unit) };

    //: Used in the statusbar showing the image size. Example: "103.4 kB"
    m_ui.statusBar->setSizeLabel(tr("%1 %2").arg(QString::number(newSize), unitString));
}

void MainWindow::onZoomPercentageChanged(const qreal value) const
{
    //: Used in the statusbar showing the zooming percentage. Example: "12%"
    m_ui.statusBar->setZoomLabel(tr("%1 %").arg(QString::number(static_cast<int>(value * 100))));
}

void MainWindow::onHomeDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(QDir::homePath())));
}

void MainWindow::onDocsDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    if (const auto locations = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::DocumentsLocation); !locations.isEmpty())
        m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(locations.first())));
}

void MainWindow::onPicturesDirClicked() const
{
    m_ui.fileSystemTreeView->collapseAll();
    if (const auto locations = QStandardPaths::standardLocations(QStandardPaths::StandardLocation::PicturesLocation); !locations.isEmpty())
        m_ui.fileSystemTreeView->setCurrentIndex(m_sortFileSystemModel->mapFromSource(m_fileSystemModel->index(locations.first())));
}
