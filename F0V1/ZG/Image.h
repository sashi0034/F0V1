#pragma once
#include "Array.h"
#include "Color.h"
#include "Value2D.h"

namespace ZG
{
    class Image
    {
    public:
        Image() = default;

        Image(const Size& size);

        ColorU8* operator[](int32_t y);

        const ColorU8* operator[](int32_t y) const;

        ColorU8& operator[](const Point& point);

        const ColorU8& operator[](const Point& point) const;

        const Size& size() const { return m_size; }

    private:
        Size m_size{};
        Array<ColorU8> m_data{};
    };
}
