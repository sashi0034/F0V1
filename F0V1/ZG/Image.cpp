#include "pch.h"
#include "Image.h"

namespace ZG
{
    Image::Image(const Size& size) : m_size(size)
    {
        m_data.resize(size.x * size.y);
    }

    ColorU8* Image::operator[](int32_t y)
    {
        return &m_data[y * m_size.x];
    }

    const ColorU8* Image::operator[](int32_t y) const
    {
        return &m_data[y * m_size.x];
    }

    ColorU8& Image::operator[](const Point& point)
    {
        return m_data[point.y * m_size.x + point.x];
    }

    const ColorU8& Image::operator[](const Point& point) const
    {
        return m_data[point.y * m_size.x + point.x];
    }
}
