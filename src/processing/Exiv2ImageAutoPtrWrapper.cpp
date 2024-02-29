/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

/// This needs to be compiled with the C++11 linker, not the newer one.

#include "Exiv2ImageAutoPtrWrapper.h"
#include <exiv2/exiv2.hpp>

std::unique_ptr<Exiv2::Image> Exiv2ImageAutoPtrWrapper::open(const std::string &path, const bool useCurl)
{
    return Exiv2::ImageFactory::open(path, useCurl);
}
