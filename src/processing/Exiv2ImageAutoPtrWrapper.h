#pragma once
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

#include <memory>
#include <string>
#include "../util/compiler.h"

namespace Exiv2
{
    class Image;
}

/// Qt requires C++17 compiler, however, the Exiv2 0.27 library relies on the std::auto_ptr,
/// which was deprecated in C++11 and later removed. This simple file will do the translation
/// between the std:unique_ptr and the std::auto_ptr.
///
/// The quick workaround could be used to leverage the Exiv2 library in C++17 environment.
///
///     namespace std
///     {
///        template<typename T>
///        using auto_ptr = std::unique_ptr<T>;
///     }
///     #include <exiv2/exiv2.hpp>
///
/// This is fine for Apple Clang, but while using the MSVC-22 compiler the library linkage
/// does not work properly due to the templates.
///
class Exiv2ImageAutoPtrWrapper
{
public:
    DISABLE_COPY_MOVE(Exiv2ImageAutoPtrWrapper);

    static std::unique_ptr<Exiv2::Image> open(const std::string& path, bool useCurl = true);
};
