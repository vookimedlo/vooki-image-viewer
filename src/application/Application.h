#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "../util/compiler.h"
#include <QApplication>
#include <QString>
#include <QTranslator>

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    DISABLE_COPY_MOVE(Application);

    void installTranslators(const QLocale &locale);

signals:
    void openFileRequested(QString path);

protected:
    bool event(QEvent *event) override;

private:
    QTranslator translator;
    QTranslator qtTranslator;
};
