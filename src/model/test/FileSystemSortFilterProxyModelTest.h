/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <array>
#include <set>
#include <QDir>
#include <QTest>

#if __has_include(<semaphore>)
    #include <semaphore>
#endif

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
    const QString m_multipleFiles {"multiple_files"};
    const QString m_multipleFilesDirPath{m_modelPath + QDir::separator() + m_multipleFiles};


    struct LessThan {
        bool operator()(const QString& a, const QString& b) const {
            return a < b;
        }
    };

    const std::set<QString, LessThan> m_multipleFilesDirectories {"Folder1",
                                                                 "folder2"};

    const std::set<QString, LessThan> m_multipleFilesFiles {"first.a_ext",
                                                            "third.a_ext",
                                                            "fourth.a_ext",
                                                            "first.b_ext",
                                                            "second.b_ext"};

#ifdef __cpp_lib_semaphore
    std::binary_semaphore m_directoryLoadedSemaphore {0};
#endif

private slots:
    void directoryLoaded(QString);

    // Test cases
    void sorting();
};
