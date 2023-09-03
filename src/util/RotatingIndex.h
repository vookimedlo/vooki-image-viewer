#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2017 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <type_traits>

template<typename T> requires std::is_integral_v<T>
class RotatingIndex
{
public:
    constexpr explicit RotatingIndex(T maxValuePlus1 = 1) :
                                            RotatingIndex {0, maxValuePlus1}
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
