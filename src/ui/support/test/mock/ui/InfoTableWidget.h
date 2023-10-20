#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QString>
#include <QTableWidget>
#include <utility>
#include <vector>

class InfoTableWidget final : public QTableWidget
{
    Q_OBJECT

public:
    explicit InfoTableWidget(QWidget *parent = nullptr) : QTableWidget(parent) {};

public slots:
    void displayInformation([[maybe_unused]] const std::vector<std::pair<QString, QString>>& information) {};
};
