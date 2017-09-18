#pragma once

#include <cstdint>
#include <QDir>
#include <QFile>
#include <QStringList>
#include "../util/RotatingIndex.h"

class ImageCatalog
{
public:
    ImageCatalog(const QStringList &filter);
    void initialize(const QFile &imageFile);
    void initialize(const QDir &imageDir);

    uint64_t getCatalogSize();
    QString getCurrent();
    QString getNext();
    QString getPrevious();

private:
    QString m_absoluteDir;
    QStringList m_catalog;
    RotatingIndex<uint64_t> m_catalogIndex;
    QStringList m_filter;
};
