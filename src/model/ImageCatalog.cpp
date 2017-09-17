#include "ImageCatalog.h"

ImageCatalog::ImageCatalog(const QStringList &filter) : m_catalogIndex(0), m_filter(filter)
{

}

void ImageCatalog::initialize(const QFile &imageFile)
{
    QFileInfo info(imageFile);
    QString filename = info.fileName();
    m_absoluteDir = info.absoluteDir().canonicalPath();
    initialize(info.absoluteDir());

    m_catalogIndex = 0;

    for (const QString& item : m_catalog)
    {
        if (item.compare(filename) == 0)
            break;
        ++m_catalogIndex;
    }
}

void ImageCatalog::initialize(const QDir &imageDir)
{
    m_absoluteDir = imageDir.canonicalPath();
    m_catalog = imageDir.entryList(m_filter, QDir::Filter::Files);
    m_catalogIndex = 0;
}

uint64_t ImageCatalog::getCatalogSize()
{
    return m_catalog.size();
}

QString ImageCatalog::getCurrent()
{
    if (m_catalog.isEmpty())
        return QString();

    return m_absoluteDir + QDir::separator() + m_catalog.at(m_catalogIndex);
}

QString ImageCatalog::getNext()
{
    if (m_catalog.isEmpty())
        return QString();

    m_catalogIndex = (m_catalogIndex + 1) % m_catalog.size();
    return m_absoluteDir + QDir::separator() + m_catalog.at(m_catalogIndex);
}

QString ImageCatalog::getPrevious()
{
    if (m_catalog.isEmpty())
        return QString();

    if (m_catalogIndex == 0)
        m_catalogIndex == m_catalog.size();

    m_catalogIndex = (m_catalogIndex - 1) % m_catalog.size();
    return m_absoluteDir + QDir::separator() + m_catalog.at(m_catalogIndex);
}
