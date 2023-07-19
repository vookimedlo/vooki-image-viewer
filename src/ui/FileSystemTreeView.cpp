/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "FileSystemTreeView.h"

#include <QKeyEvent>

FileSystemTreeView::FileSystemTreeView(QWidget *parent)
                                        : QTreeView(parent)
{
    QObject::connect(this, &QTreeView::expanded, this, &FileSystemTreeView::onExpanded, Qt::QueuedConnection);
}

FileSystemTreeView::~FileSystemTreeView()
{
    QObject::disconnect(this, &QTreeView::expanded, this, &FileSystemTreeView::onExpanded);
}

void FileSystemTreeView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Down:
            [[fallthrough]];
        case Qt::Key_Up:
            [[fallthrough]];
        case Qt::Key_Left:
            [[fallthrough]];
        case Qt::Key_Right:
            [[fallthrough]];
        case Qt::Key_PageUp:
            [[fallthrough]];
        case Qt::Key_PageDown:
            QTreeView::keyPressEvent(event);
            break;
        case Qt::Key_Enter:
            [[fallthrough]];
        case Qt::Key_Return:
            emit activated(currentIndex());
            break;
        default:
            // Ignore the key and send it up to the parent widget
            event->ignore();
    }
}

void FileSystemTreeView::setCurrentIndex(const QModelIndex &index)
{
    QAbstractItemView::setCurrentIndex(index);
    // Guard is not needed here, since an execution is always synchronized by the Qt::QueuedConnection signal processing
    m_setIndex = index;
    setExpanded(index, true);
}

void FileSystemTreeView::onExpanded(const QModelIndex &index)
{
    if (m_setIndex == index)
        scrollTo(currentIndex(), QAbstractItemView::PositionAtCenter);
}
