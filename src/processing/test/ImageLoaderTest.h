/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QTest>

#ifdef __APPLE__
    #define PREFIX(X) "../../../" X
#else
    #define PREFIX(X) X
#endif

class ImageLoaderTest: public QObject
{
    Q_OBJECT

    QString makeAbsolutePath(const QString &file) const;
    static constexpr const char *png1FilePath = PREFIX("1.png");
    static constexpr const char *animatedNumbersFilePath = PREFIX("animated_numbers.webp");


    const QString m_absolutePath {QCoreApplication::applicationDirPath()};

private slots:
    void open() const;
    void getImageNotAnimated() const;
    void getImageAnimated() const;
};
