/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "ArrayTest.h"
#include "ByteSizeTest.h"
#include "MiscTest.h"
#include "EnumClassArrayTest.h"
#include "RotatingIndexTest.h"
#include "../testing.h"


int main(int argc, char *argv[])
{
    int status = 0;

    TEST::runTests<ArrayTest>(argc, argv, &status);
    TEST::runTests<ByteSizeTest>(argc, argv, &status);
    TEST::runTests<EnumClassArrayTest>(argc, argv, &status);
    TEST::runTests<MiscTest>(argc, argv, &status);
    TEST::runTests<RotatingIndexTest>(argc, argv, &status);

    return status;
}
