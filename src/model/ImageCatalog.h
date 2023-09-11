#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/RotatingIndex.h"
#include "../util/compiler.h"
#include <QDir>
#include <QFile>
#include <QStringList>
#include <cstdint>

class ImageCatalog
{
public:
    explicit ImageCatalog(QStringList filter);
    DISABLE_COPY_MOVE(ImageCatalog);

    void initialize(const QFile &imageFile);
    void initialize(const QDir &imageDir);

    [[nodiscard]] qsizetype getCatalogSize() const;
    [[nodiscard]] QString getCurrent() const;
    [[nodiscard]] QString getNext();
    [[nodiscard]] QString getPrevious();

protected:
    [[nodiscard]] QString getCatalogItem(const RotatingIndex<QIntegerForSizeof<std::size_t>::Unsigned> &catalogIndex) const;

private:
    QString m_absoluteDir;
    QStringList m_catalog;
    RotatingIndex<QIntegerForSizeof<std::size_t>::Unsigned> m_catalogIndex;
    QStringList m_filter;
};
