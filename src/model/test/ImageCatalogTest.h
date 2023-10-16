/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <array>
#include <QDir>
#include <QTest>

#ifdef __APPLE__
    #define PREFIX(X) "../../../" X
#else
    #define PREFIX(X) X
#endif

class ImageCatalogTest: public QObject
{
    Q_OBJECT

    QString makeAbsolutePath(const QString &file) const;

    const QString m_absolutePath {QCoreApplication::applicationDirPath()};
    const QString m_modelPath {PREFIX("model")};
    const QString singleFileDirPath {m_modelPath + QDir::separator() + "single_file"};
    const QString singleFilePath {singleFileDirPath + QDir::separator() + "first.first_ext"};
    const QString multipleFilesDirPath {m_modelPath + QDir::separator() + "multiple_files"};
    const QString fourthFilePath{multipleFilesDirPath + QDir::separator() + "fourth.a_ext"};

    const std::array<QString, 3> multipleFilesExtA {multipleFilesDirPath + QDir::separator() + "first.a_ext", multipleFilesDirPath + QDir::separator() + "third.a_ext", multipleFilesDirPath + QDir::separator() + "fourth.a_ext"};
    const std::array<QString, 2> multipleFilesExtB {multipleFilesDirPath + QDir::separator() + "first.b_ext", multipleFilesDirPath + QDir::separator() + "second.b_ext"};

private slots:
    void noInitialization() const;
    void initializationWithNonExistingDir() const;
    void initializationWithNonExistingFile() const;
    void initializationWithExistingFile() const;
    void initializationWithExistingFileFiltered() const;
    void initializationWithExistingFileNegativeFiltered() const;
    void initializationWithExistingDir() const;
    void initializationWithExistingDirExtBFiltered() const;
    void initializationWithExistingFileExtBFiltered() const;
};
