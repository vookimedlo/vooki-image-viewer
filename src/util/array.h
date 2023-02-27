#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2023  Michal Duda <github@vookimedlo.cz>

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

#include <algorithm>
#include <array>

namespace Array
{
    namespace Helper
    {
        template<typename BASE, typename T1, typename T2, std::size_t... I1, std::size_t... I2>
            requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                     std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> && std::is_pointer_v<T1> && std::is_pointer_v<T2>
        constexpr std::array<BASE, sizeof...(I1) + sizeof...(I2)> construct_array_impl(const std::array<T1, sizeof...(I1)> &array1,
                                                                                       std::index_sequence<I1...>,
                                                                                       const std::array<T2, sizeof...(I2)> &array2,
                                                                                       std::index_sequence<I2...>)
        {
            return { { array1[I1]..., array2[I2]... } };
        }

        template<typename BASE, std::size_t N1, std::size_t N2, typename T1, typename T2>
            requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                     std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> && std::is_pointer_v<T1> && std::is_pointer_v<T2>
        constexpr std::array<BASE, N1 + N2> construct_array(const std::array<T1, N1> &array1, const std::array<T2, N2> &array2)
        {
            return construct_array_impl<BASE, T1, T2>(array1, std::make_index_sequence<N1>{}, array2, std::make_index_sequence<N2>{});
        }
    }

    /// Joins derived arrays so all elements are copied over to the base array. Supports (T* const) qualifier.
    template<typename BASE, typename T1, std::size_t N1, typename T2, std::size_t N2>
        requires std::derived_from<std::remove_pointer_t<T1>, std::remove_pointer_t<BASE>> &&
                 std::derived_from<std::remove_pointer_t<T2>, std::remove_pointer_t<BASE>> &&
                 std::is_pointer_v<T1> && std::is_pointer_v<T2>
    constexpr std::array<BASE, N1 + N2> concatenate(const std::array<T1, N1> &array1, const std::array<T2, N2> &array2)
    {
        return Helper::construct_array<BASE>(array1, array2);
    }
}
