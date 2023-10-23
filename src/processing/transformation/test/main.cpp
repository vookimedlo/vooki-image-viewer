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

    TEST::runTests<ImageBorderTest>(argc, argv, &status);
    TEST::runTests<ImageFlipTest>(argc, argv, &status);
    TEST::runTests<ImageRotationTest>(argc, argv, &status);
    TEST::runTests<ImageTransformationBaseTest>(argc, argv, &status);
    TEST::runTests<ImageZoomTest>(argc, argv, &status);

    return status;
}
