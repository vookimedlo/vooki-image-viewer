/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "FileSystemSortFilterProxyModelTest.h"

#include <QFileSystemModel>
#include <ranges>
#include <unordered_set>
#include "../FileSystemSortFilterProxyModel.h"
#include "../../util/array.h"

QString FileSystemSortFilterProxyModelTest::makeAbsolutePath(const QString &file) const
{
    return QDir::cleanPath(m_absolutePath + QDir::separator() + file);
}

void FileSystemSortFilterProxyModelTest::directoryLoaded([[maybe_unused]] QString path)
{
#ifdef __cpp_binary_literals
    m_directoryLoadedSemaphore.release();
#endif
}

void FileSystemSortFilterProxyModelTest::sorting()
{
    QFileSystemModel model{};
    connect(&model, SIGNAL(directoryLoaded(QString)), this, SLOT(directoryLoaded(QString)));
    model.setFilter(QDir::Filter::Hidden | QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot | QDir::Filter::AllDirs);
    model.setNameFilters({ "*" });
    model.setNameFilterDisables(false);
    auto modelIndex = model.setRootPath(makeAbsolutePath(m_multipleFilesDirPath));

#ifdef __cpp_binary_literals
    // Model is populated in the dedicated thread so we need to wait for its completion.
    bool isDirectoryLoaded {QTest::qWaitFor([&directoryLoadedSemaphore = m_directoryLoadedSemaphore]() {
        return directoryLoadedSemaphore.try_acquire();
    }, 3000)};
#else
    QTest::qWait(3000);
#endif

    disconnect(&model, SIGNAL(directoryLoaded(QString)), this, SLOT(directoryLoaded(QString)));
    QCOMPARE(isDirectoryLoaded, true);

    FileSystemSortFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&model);
    proxyModel.sort(0, Qt::SortOrder::AscendingOrder);

    std::vector<QString> expectedMultipleFilesContentOrdered;
    std::ranges::copy(m_multipleFilesDirectories, std::back_inserter(expectedMultipleFilesContentOrdered));
    std::ranges::copy(m_multipleFilesFiles, std::back_inserter(expectedMultipleFilesContentOrdered));
    const std::unordered_set<QString> expectedMultipleFilesContent {expectedMultipleFilesContentOrdered.begin(), expectedMultipleFilesContentOrdered.end()};

    QCOMPARE(model.rowCount(modelIndex), expectedMultipleFilesContent.size());
    QCOMPARE(model.data(modelIndex).value<QString>(), m_multipleFiles);
    QCOMPARE(proxyModel.rowCount(proxyModel.mapFromSource(modelIndex)), expectedMultipleFilesContent.size());
    QCOMPARE(proxyModel.data(proxyModel.mapFromSource(modelIndex)).value<QString>(), m_multipleFiles);

    std::unordered_set<QString> multipleFilesContent {};
    for (auto row = 0; row < model.rowCount(modelIndex); ++row)
        multipleFilesContent.insert(model.data(model.index(row, 0, modelIndex)).value<QString>());
    QCOMPARE(multipleFilesContent, expectedMultipleFilesContent);

    multipleFilesContent.clear();
    for (auto row = 0; row < proxyModel.rowCount(proxyModel.mapFromSource(modelIndex)); ++row)
        multipleFilesContent.insert(proxyModel.data(proxyModel.index(row, 0, proxyModel.mapFromSource(modelIndex))).value<QString>());
    QCOMPARE(multipleFilesContent, expectedMultipleFilesContent);

    std::vector<QString> multipleFilesContentOrdered;
    for (auto row = 0; row < proxyModel.rowCount(proxyModel.mapFromSource(modelIndex)); ++row)
        multipleFilesContentOrdered.push_back(proxyModel.data(proxyModel.index(row, 0, proxyModel.mapFromSource(modelIndex))).value<QString>());
    QCOMPARE(multipleFilesContentOrdered, expectedMultipleFilesContentOrdered);
}
