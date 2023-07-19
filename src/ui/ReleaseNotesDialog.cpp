/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ReleaseNotesDialog.h"

ReleaseNotesDialog::ReleaseNotesDialog(QWidget *parent)
                                        : QDialog(parent)
{
    m_uiReleaseNotesDialog.setupUi(this);
    const auto &changelog = m_uiReleaseNotesDialog.textBrowser->loadResource(QTextDocument::MarkdownResource,
                                                                      QUrl("qrc:/text/changelog"));
    m_uiReleaseNotesDialog.textBrowser->setMarkdown(changelog.toString());
}
