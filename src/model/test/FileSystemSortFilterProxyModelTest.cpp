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

void FileSystemSortFilterProxyModelTest::directoryLoaded(QString f)
{
    m_directoryLoadedSemaphore.release();
}

void FileSystemSortFilterProxyModelTest::sorting()
{
    QFileSystemModel model{};
    connect(&model, SIGNAL(directoryLoaded(QString)), this, SLOT(directoryLoaded(QString)));
    model.setFilter(QDir::Filter::Hidden | QDir::Filter::AllEntries | QDir::Filter::NoDotAndDotDot | QDir::Filter::AllDirs);
    model.setNameFilters({ "*" });
    model.setNameFilterDisables(false);
    auto modelIndex = model.setRootPath(makeAbsolutePath(m_multipleFilesDirPath));

    // Model is populated in the dedicated thread so we need to wait for its completion.
    while (!m_directoryLoadedSemaphore.try_acquire())
        QTest::qWait(100);

    FileSystemSortFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&model);
    proxyModel.sort(0, Qt::SortOrder::AscendingOrder);

    QCOMPARE(model.rowCount(modelIndex), m_multipleFilesContent.size());
    QCOMPARE(model.data(modelIndex).value<QString>(), "multiple_files");
    QCOMPARE(proxyModel.rowCount(proxyModel.mapFromSource(modelIndex)), m_multipleFilesContent.size());
    QCOMPARE(proxyModel.data(proxyModel.mapFromSource(modelIndex)).value<QString>(), "multiple_files");

    std::unordered_set<QString> expectedMultipleFilesContent {m_multipleFilesContent.cbegin(), m_multipleFilesContent.cend()};
    std::unordered_set<QString> multipleFilesContent {};

    for (auto row = 0; row < model.rowCount(modelIndex); ++row)
        multipleFilesContent.insert(m_multipleFilesDirPath + QDir::separator() + model.data(model.index(row, 0, modelIndex)).value<QString>());

    QCOMPARE(multipleFilesContent, expectedMultipleFilesContent);

    multipleFilesContent.clear();
    for (auto row = 0; row < proxyModel.rowCount(proxyModel.mapFromSource(modelIndex)); ++row)
        multipleFilesContent.insert(m_multipleFilesDirPath + QDir::separator() + proxyModel.data(proxyModel.index(row, 0, proxyModel.mapFromSource(modelIndex))).value<QString>());

    QCOMPARE(multipleFilesContent, expectedMultipleFilesContent);
}
