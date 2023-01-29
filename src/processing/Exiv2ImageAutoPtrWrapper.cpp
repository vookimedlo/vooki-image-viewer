/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2022  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

/// This needs to be compiled with the C++11 linker, not the newer one.

#include "Exiv2ImageAutoPtrWrapper.h"
#include <exiv2/exiv2.hpp>

std::unique_ptr<Exiv2::Image> Exiv2ImageAutoPtrWrapper::open(const std::string &path, bool useCurl)
{
    return Exiv2::ImageFactory::open(path, useCurl);
}
