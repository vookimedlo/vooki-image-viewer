/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "RotatingIndexTest.h"
#include "../testing.h"


int main(int argc, char *argv[])
{
    int status = 0;

    runTests<RotatingIndexTest>(argc, argv, &status);

    return status;
}
