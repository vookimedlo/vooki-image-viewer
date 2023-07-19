/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

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
            showResourceMarkdown("qrc:/text/aboutbrotli");
            break;
        case 1:
            showResourceMarkdown("qrc:/text/aboutexiv2");
            break;
        case 2:
            showResourceMarkdown("qrc:/text/aboutexpat");
            break;
        case 3:
            showResourceMarkdown("qrc:/text/abouthighway");
            break;
        case 4:
            showResourceMarkdown("qrc:/text/abouticon54");
            break;
        case 5:
            showResourceMarkdown("qrc:/text/aboutkimageformats");
            break;
        case 6:
            showResourceMarkdown("qrc:/text/aboutkdmactouchbar");
            break;
        case 7:
            showResourceMarkdown("qrc:/text/aboutlibde265");
            break;
        case 8:
            showResourceMarkdown("qrc:/text/aboutlibheif");
            break;
        case 9:
            showResourceMarkdown("qrc:/text/aboutlibjxl");
            break;
        case 10:
            showResourceMarkdown("qrc:/text/aboutlibjpegturbo");
            break;
        case 11:
            showResourceMarkdown("qrc:/text/aboutlibraw");
            break;
        case 12:
            showResourceMarkdown("qrc:/text/aboutopenclipart");
            break;
        case 13:
            showResourceMarkdown("qrc:/text/aboutqcoro");
            break;
        case 14:
            showResourceMarkdown("qrc:/text/aboutqt");
            break;
        case 15:
            showResourceMarkdown("qrc:/text/aboutzlib");
            break;
        default:
            m_uiAboutComponentsDialog.textBrowser->clear();
    }
}

void AboutComponentsDialog::showResourceMarkdown(const QString &resource)
{
    const auto &markdown = m_uiAboutComponentsDialog.textBrowser->loadResource(QTextDocument::MarkdownResource,
                                                                              QUrl(resource));
    m_uiAboutComponentsDialog.textBrowser->setMarkdown(markdown.toString());
}
