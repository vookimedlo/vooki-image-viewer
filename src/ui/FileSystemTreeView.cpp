/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "FileSystemTreeView.h"

#include <QKeyEvent>

FileSystemTreeView::FileSystemTreeView(QWidget *parent) : QTreeView(parent)
{
    QObject::connect(this, &QTreeView::expanded, this, &FileSystemTreeView::onExpanded, Qt::QueuedConnection);
}

FileSystemTreeView::~FileSystemTreeView()
{
    QObject::disconnect(this, &QTreeView::expanded, this, &FileSystemTreeView::onExpanded);
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
    if(m_setIndex == index)
        scrollTo(currentIndex(), QAbstractItemView::PositionAtCenter);
}

void FileSystemTreeView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
        QTreeView::keyPressEvent(event);
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        emit activated(currentIndex());
        break;
    default:
        // Ignore the key and send it up to the parent widget
        event->ignore();
    }
}
