/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ImageBorderTest.h"
#include "ImageFlipTest.h"
#include "ImageRotationTest.h"
#include "ImageTransformationBaseTest.h"
#include "ImageZoomTest.h"

#include "../../../util/testing.h"


int main(int argc, char *argv[])
{
    int status = 0;

    runTests<ImageBorderTest>(argc, argv, &status);
    runTests<ImageFlipTest>(argc, argv, &status);
    runTests<ImageRotationTest>(argc, argv, &status);
    runTests<ImageTransformationBaseTest>(argc, argv, &status);
    runTests<ImageZoomTest>(argc, argv, &status);

    return status;
}
