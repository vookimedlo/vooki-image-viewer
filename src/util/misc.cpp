#include "misc.h"

namespace Util
{
    QStringList convertFormatsToFilters(const QList<QByteArray>& formats)
    {
        QStringList filters;
        // Converts filters (e.g. QImageReader::supportedImageFormats()) to QDir::setNameFilters()
        for (const QByteArray &format : formats)
        {
            QString filter("*.");
            filter += QString::fromLatin1(format.toLower());
            filters << filter;
        }
        return filters;
    }
}
