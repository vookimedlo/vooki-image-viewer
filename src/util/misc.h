#pragma once

#include <QByteArray>
#include <QList>
#include <QStringList>

namespace Util
{
    QStringList convertFormatsToFilters(const QList<QByteArray>& formats);
}
