#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
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

#include <type_traits>

template<typename T> requires std::is_integral_v<T>
class RotatingIndex
{
public:
    constexpr explicit RotatingIndex(T initialValue = 0) :
                                            RotatingIndex {0, initialValue + 1}
    {
    }

    constexpr RotatingIndex(T initialValue, T maxValuePlus1) :
                                            m_index {initialValue},
                                            m_maxValuePlus1(maxValuePlus1)
    {
    }

    constexpr bool set(T initialValue, T maxValuePlus1)
    {
        // values must be in this relation: initialValue < maxValuePlus1
        if (initialValue < maxValuePlus1)
        {
            m_index = initialValue;
            m_maxValuePlus1 = maxValuePlus1;
            return true;
        }
        return false;
    }

    constexpr bool reset(T indexValue)
    {
        if (indexValue < m_maxValuePlus1)
        {
            m_index = indexValue;
            return true;
        }
        return false;
    }

    // Implicit conversion
    operator T() const { return m_index; }

    // Pre-increment operator
    RotatingIndex &operator++()
    {
        m_index = (m_index + 1) % m_maxValuePlus1;
        return *this;
    }

    // Post-increment operator
    RotatingIndex operator++(int)
    {
        const RotatingIndex old(*this);
        operator++();
        return old;
    }

    // Pre-decrement operator
    RotatingIndex &operator--()
    {
        if (m_index == 0)
            m_index = m_maxValuePlus1;

        m_index = (m_index - 1) % m_maxValuePlus1;
        return *this;
    }

    // Post-decrement operator
    RotatingIndex operator--(int)
    {
        const RotatingIndex old(*this);
        operator--();
        return old;
    }

private:
    T m_index;
    T m_maxValuePlus1;
};
