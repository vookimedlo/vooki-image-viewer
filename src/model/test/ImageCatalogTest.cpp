/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageCatalogTest.h"

#include <ranges>
#include "../ImageCatalog.h"
#include "../../util/array.h"

QString ImageCatalogTest::makeAbsolutePath(const QString &file) const
{
    return QDir::cleanPath(m_absolutePath + QDir::separator() + file);
}

void ImageCatalogTest::noInitialization() const
{
    ImageCatalog imageCatalog {QStringList {}};
    QCOMPARE(imageCatalog.getCurrent(), QString{});
    QCOMPARE(imageCatalog.getNext(), QString{});
    QCOMPARE(imageCatalog.getPrevious(), QString{});
    QCOMPARE(imageCatalog.getCatalogSize(), 0);
}

void ImageCatalogTest::initializationWithNonExistingDir() const
{
    ImageCatalog imageCatalog {QStringList {}};
    imageCatalog.initialize(QDir{"@#$"});
    QCOMPARE(imageCatalog.getCurrent(), QString{});
    QCOMPARE(imageCatalog.getNext(), QString{});
    QCOMPARE(imageCatalog.getPrevious(), QString{});
    QCOMPARE(imageCatalog.getCatalogSize(), 0);
}

void ImageCatalogTest::initializationWithNonExistingFile() const
{
    ImageCatalog imageCatalog {QStringList {}};
    imageCatalog.initialize(QFile{"/@#$#$#$/@#$"});
    QCOMPARE(imageCatalog.getCurrent(), QString{});
    QCOMPARE(imageCatalog.getNext(), QString{});
    QCOMPARE(imageCatalog.getPrevious(), QString{});
    QCOMPARE(imageCatalog.getCatalogSize(), 0);
}

void ImageCatalogTest::initializationWithExistingFile() const
{
    ImageCatalog imageCatalog {QStringList {}};
    imageCatalog.initialize(QDir(ImageCatalogTest::makeAbsolutePath(m_singleFileDirPath)));

    const auto expectedFilePath {ImageCatalogTest::makeAbsolutePath(m_singleFilePath)};
    QCOMPARE(imageCatalog.getCurrent(), expectedFilePath);
    QCOMPARE(imageCatalog.getNext(), expectedFilePath);
    QCOMPARE(imageCatalog.getPrevious(), expectedFilePath);
    QCOMPARE(imageCatalog.getCatalogSize(), 1);
}

void ImageCatalogTest::initializationWithExistingFileFiltered() const
{
    for (const auto &filter: {QStringList{"*.first_ext"}, QStringList{"*.first_*"}, QStringList{"*.first_ext"}, QStringList{"*.*_ext"}, QStringList{"*"}, QStringList{"f*"}})
    {
        ImageCatalog imageCatalog{ filter };
        imageCatalog.initialize(QDir(ImageCatalogTest::makeAbsolutePath(m_singleFileDirPath)));

        const auto expectedFilePath{ ImageCatalogTest::makeAbsolutePath(m_singleFilePath) };
        QCOMPARE(imageCatalog.getCurrent(), expectedFilePath);
        QCOMPARE(imageCatalog.getNext(), expectedFilePath);
        QCOMPARE(imageCatalog.getPrevious(), expectedFilePath);
        QCOMPARE(imageCatalog.getCatalogSize(), 1);
    }
}

void ImageCatalogTest::initializationWithExistingFileNegativeFiltered() const
{
    for (const auto &filter: {QStringList{"*.first_ext1"}, QStringList{"*.first"}, QStringList{"1.first_ext"}, QStringList{"*f"}})
    {
        ImageCatalog imageCatalog{ filter };
        imageCatalog.initialize(QDir(ImageCatalogTest::makeAbsolutePath(m_singleFileDirPath)));

        QCOMPARE(imageCatalog.getCurrent(), QString{});
        QCOMPARE(imageCatalog.getNext(), QString{});
        QCOMPARE(imageCatalog.getPrevious(), QString{});
        QCOMPARE(imageCatalog.getCatalogSize(), 0);
    }
}

void ImageCatalogTest::initializationWithExistingDir() const
{
    for (const auto &list: {QStringList {}, QStringList {"*"}, QStringList {"*.*"}, QStringList {"BLAH", "*.*"}})
    {
        ImageCatalog imageCatalog{ list };
        imageCatalog.initialize(QDir(ImageCatalogTest::makeAbsolutePath(m_multipleFilesDirPath)));

        auto expectedFiles{ Array::concatenate<QString>(m_multipleFilesExtA, m_multipleFilesExtB) };
        std::ranges::transform(expectedFiles, expectedFiles.begin(), [absolutePath = m_absolutePath](const QString &s) {
            return QDir::cleanPath(absolutePath + QDir::separator() + s);
        });
        std::ranges::sort(expectedFiles, [](const QString &left, const QString &right) { return left < right; });
        QCOMPARE(imageCatalog.getCatalogSize(), expectedFiles.size());

        for (const auto &expectedFile : expectedFiles)
        {
            QCOMPARE(imageCatalog.getCurrent(), expectedFile);
            imageCatalog.getNext();
        }

        std::ranges::for_each(expectedFiles.rbegin(), expectedFiles.rend(), [&imageCatalog](const auto &expectedFile) {
            imageCatalog.getPrevious();
            QCOMPARE(imageCatalog.getCurrent(), expectedFile);
        });
    }
}

void ImageCatalogTest::initializationWithExistingDirExtBFiltered() const
{
    ImageCatalog imageCatalog {{"*.b_ext"}};
    imageCatalog.initialize(QDir(ImageCatalogTest::makeAbsolutePath(m_multipleFilesDirPath)));

    auto expectedFiles { m_multipleFilesExtB };
    std::ranges::transform(expectedFiles, expectedFiles.begin(), [absolutePath = m_absolutePath](const QString& s) { return QDir::cleanPath(absolutePath + QDir::separator() + s); });

    QCOMPARE(imageCatalog.getCatalogSize(), expectedFiles.size());

    for (const auto &expectedFile: expectedFiles)
    {
        QCOMPARE(imageCatalog.getCurrent(), expectedFile);
        imageCatalog.getNext();
    }

    std::ranges::for_each(expectedFiles.rbegin(), expectedFiles.rend(), [&imageCatalog](const auto &expectedFile) {
        imageCatalog.getPrevious();
        QCOMPARE(imageCatalog.getCurrent(), expectedFile);
    });
}

void ImageCatalogTest::initializationWithExistingFileExtBFiltered() const
{
    ImageCatalog imageCatalog {{"*.a_ext"}};
    imageCatalog.initialize(QFile(ImageCatalogTest::makeAbsolutePath(m_fourthFilePath)));

    auto expectedFiles { m_multipleFilesExtA };
    std::ranges::transform(expectedFiles, expectedFiles.begin(), [absolutePath = m_absolutePath](const QString& s) { return QDir::cleanPath(absolutePath + QDir::separator() + s); });

    QCOMPARE(imageCatalog.getCatalogSize(), expectedFiles.size());

    std::ranges::for_each(expectedFiles.rbegin(), expectedFiles.rend(), [&imageCatalog](const auto &expectedFile) {
        QCOMPARE(imageCatalog.getCurrent(), expectedFile);
        imageCatalog.getNext();
    });

    for (const auto &expectedFile: expectedFiles)
    {
        imageCatalog.getPrevious();
        QCOMPARE(imageCatalog.getCurrent(), expectedFile);
    }
}
