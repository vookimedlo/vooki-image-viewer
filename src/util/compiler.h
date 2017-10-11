#pragma once
/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2017  Michal Duda <github@vookimedlo.cz>

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

/*
 * C++11 Draft
 * - url http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4296.pdf
 * $5.2.9/6 Static cast
 *   Any expression can be explicitly converted to type cv void, in which case it becomes a discarded - value
 *   expression(Clause 5).[Note:however, if the value is in a temporary object(12.2), the destructor for that
 *   object is not executed until the usual time, and the value of the object is preserved for the purpose of
 *   executing the destructor.
 */
#define UNUSED_VARIABLE(X) static_cast<const void>(X)

#define DISABLE_COPY_MOVE(CLASS)                                                                                                                               \
    CLASS &operator=(const CLASS &) = delete;                                                                                                                  \
    CLASS(const CLASS &) = delete;                                                                                                                             \
    CLASS &operator=(const CLASS &&) = delete;                                                                                                                 \
    CLASS(const CLASS &&) = delete
