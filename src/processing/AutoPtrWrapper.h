#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <memory>

/// Qt requires C++17 compiler, however, the Exiv2 0.27 library relies on the std::auto_ptr,
/// which was deprecated in C++11 and later removed. This simple file will do the translation
/// between the std:unique_ptr and the std::auto_ptr.
///
/// The quick workaround could be used to leverage the Exiv2 library in C++17 environment.
///
#if defined (__APPLE__) or defined (_WIN32)
namespace std
{
    template<typename T>
    using auto_ptr = std::unique_ptr<T>;
}
#endif
