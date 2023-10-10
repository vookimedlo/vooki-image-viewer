/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageLoaderTest.h"

#include "../../util/testing.h"


int main(int argc, char *argv[])
{
    int status = 0;

    runTests<ImageLoaderTest>(argc, argv, &status);

    return status;
}
