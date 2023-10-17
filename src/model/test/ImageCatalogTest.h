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
    const QString m_singleFileDirPath{m_modelPath + QDir::separator() + "single_file"};
    const QString m_singleFilePath{ m_singleFileDirPath + QDir::separator() + "first.first_ext"};
    const QString m_multipleFilesDirPath{m_modelPath + QDir::separator() + "multiple_files"};
    const QString m_fourthFilePath{ m_multipleFilesDirPath + QDir::separator() + "fourth.a_ext"};

    const std::array<QString, 3> m_multipleFilesExtA{ m_multipleFilesDirPath + QDir::separator() + "first.a_ext",
                                                      m_multipleFilesDirPath + QDir::separator() + "third.a_ext",
                                                      m_multipleFilesDirPath + QDir::separator() + "fourth.a_ext"};
    const std::array<QString, 2> m_multipleFilesExtB{ m_multipleFilesDirPath + QDir::separator() + "first.b_ext",
                                                      m_multipleFilesDirPath + QDir::separator() + "second.b_ext"};

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
