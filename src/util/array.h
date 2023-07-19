#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <algorithm>
#include <array>

namespace Array
{
    namespace Helper
    {
        template<typename BASE, typename T1, typename T2, std::size_t... I1, std::size_t... I2>
            requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                     std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> && std::is_pointer_v<T1> && std::is_pointer_v<T2>
        std::array<BASE, sizeof...(I1) + sizeof...(I2)> construct_array_impl(const std::array<T1, sizeof...(I1)> &array1,
                                                                             std::index_sequence<I1...>,
                                                                             const std::array<T2, sizeof...(I2)> &array2,
                                                                             std::index_sequence<I2...>)
        {
            return { { array1[I1]..., array2[I2]... } };
        }

        template<typename BASE, std::size_t N1, std::size_t N2, typename T1, typename T2>
            requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                     std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> && std::is_pointer_v<T1> && std::is_pointer_v<T2>
        std::array<BASE, N1 + N2> construct_array(const std::array<T1, N1> &array1, const std::array<T2, N2> &array2)
        {
            return construct_array_impl<BASE, T1, T2>(array1, std::make_index_sequence<N1>{}, array2, std::make_index_sequence<N2>{});
        }
    }

    /// Joins derived arrays so all elements are copied over to the base array. Supports (T* const) qualifier.
    template<typename BASE, typename T1, std::size_t N1, typename T2, std::size_t N2>
        requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                 std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> &&
                 std::is_pointer_v<T1> && std::is_pointer_v<T2>
    std::array<BASE, N1 + N2> concatenate(const std::array<T1, N1> &array1, const std::array<T2, N2> &array2)
    {
        return Helper::construct_array<BASE>(array1, array2);
    }
}
