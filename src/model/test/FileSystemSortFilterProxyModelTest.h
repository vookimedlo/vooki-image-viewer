/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <array>
#include <semaphore>
#include <QDir>
#include <QTest>

#ifdef __APPLE__
    #define PREFIX(X) "../../../" X
#else
    #define PREFIX(X) X
#endif

class FileSystemSortFilterProxyModelTest: public QObject
{
    Q_OBJECT

    QString makeAbsolutePath(const QString &file) const;

    const QString m_absolutePath {QCoreApplication::applicationDirPath()};
    const QString m_modelPath {PREFIX("model")};
    const QString m_multipleFilesDirPath{m_modelPath + QDir::separator() + "multiple_files"};

    const std::array<QString, 7> m_multipleFilesContent {m_multipleFilesDirPath + QDir::separator() + "Folder1",
                                                         m_multipleFilesDirPath + QDir::separator() + "folder2",
                                                         m_multipleFilesDirPath + QDir::separator() + "first.a_ext",
                                                         m_multipleFilesDirPath + QDir::separator() + "third.a_ext",
                                                         m_multipleFilesDirPath + QDir::separator() + "fourth.a_ext",
                                                         m_multipleFilesDirPath + QDir::separator() + "first.b_ext",
                                                         m_multipleFilesDirPath + QDir::separator() + "second.b_ext"};

    std::binary_semaphore m_directoryLoadedSemaphore {0};


private slots:
    void directoryLoaded(QString);

    // Test cases
    void sorting();
};
