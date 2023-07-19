#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <array>

template <class T, std::size_t N>
struct EnumClassArray : std::array<T, N>
{
    template <typename I>
    T& operator[](const I& i) {
        return std::array<T, N>::operator[](static_cast<typename std::underlying_type_t<I>>(i));
    }

    template <typename I>
    const T& operator[](const I& i) const {
        return std::array<T, N>::operator[](static_cast<typename std::underlying_type_t<I>>(i));
    }
};
