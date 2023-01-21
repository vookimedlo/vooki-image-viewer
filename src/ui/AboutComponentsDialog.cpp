/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include "AboutComponentsDialog.h"

AboutComponentsDialog::AboutComponentsDialog(QWidget *parent)
                                        : QDialog(parent)
{
    m_uiAboutComponentsDialog.setupUi(this);
    m_uiAboutComponentsDialog.listWidget->setCurrentRow(0);
}

void AboutComponentsDialog::onSelectedComponentChanged(const int row)
{
    switch (row)
    {
        case 0:
            showResourceMarkdown("qrc:/text/aboutqt");
            break;
        case 1:
            showResourceMarkdown("qrc:/text/abouticon54");
            break;
        case 2:
            showResourceMarkdown("qrc:/text/aboutopenclipart");
            break;
        case 3:
            showResourceMarkdown("qrc:/text/aboutkimageformats");
            break;
        case 4:
            showResourceMarkdown("qrc:/text/aboutlibraw");
            break;
        case 5:
            showResourceMarkdown("qrc:/text/aboutlibde265");
            break;
        case 6:
            showResourceMarkdown("qrc:/text/aboutlibheif");
            break;
        case 7:
            showResourceMarkdown("qrc:/text/aboutx265");
            break;
        case 8:
            showResourceMarkdown("qrc:/text/aboutkdmactouchbar");
            break;
        case 9:
            showResourceMarkdown("qrc:/text/aboutlibjpegturbo");
            break;
        case 10:
            showResourceMarkdown("qrc:/text/aboutzlib");
            break;
        default:
            m_uiAboutComponentsDialog.textBrowser->clear();
    }
}

void AboutComponentsDialog::showResourceMarkdown(const QString &resource)
{
    const auto markdown = m_uiAboutComponentsDialog.textBrowser->loadResource(QTextDocument::MarkdownResource,
                                                                              QUrl(resource));
    m_uiAboutComponentsDialog.textBrowser->setMarkdown(markdown.toString());
}
