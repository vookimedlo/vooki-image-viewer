/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "misc.h"
#include <QAction>
#include <QStringBuilder>
#include <queue>

namespace Util
{
    QStringList convertFormatsToFilters(const QList<QByteArray> &formats)
    {
        QStringList filters;
        // Converts formats (e.g. QImageReader::supportedImageFormats()) to QDir::setNameFilters()
        for (const QByteArray &format : formats)
            filters << "*." % QString::fromLatin1(format.toLower());
        return filters;
    }

    std::vector<const QAction *> getAllActionsHavingShortcut(const QMenu *menu)
    {
        Q_ASSERT(menu);

        std::vector<const QAction *> result;
        std::queue<const QMenu *> unprocessedMenus;
        unprocessedMenus.push(menu);

        while (!unprocessedMenus.empty())
        {
            const QMenu * const unprocessedMenu = unprocessedMenus.front();
            unprocessedMenus.pop();

            for (const auto *const action : unprocessedMenu->actions())
            {
                if (action->isSeparator())
                    continue;

                if (action->menu() && action->menu() != unprocessedMenu)
                {
                    unprocessedMenus.push(action->menu());
                    continue;
                }

                if (action->whatsThis().isEmpty())
                    continue;

                result.push_back(action);
            }
        }

        return result;
    }
}
