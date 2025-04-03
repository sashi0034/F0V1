#pragma once
#include "Value2D.h"

namespace ZG
{
    struct RectF
    {
        using value_type = double;
        using position_type = Vec2;

        position_type pos;
        position_type size;

        [[nodiscard]]
        RectF() = default;

        [[nodiscard]]
        RectF(const position_type& pos, const position_type& size)
            : pos(pos), size(size)
        {
        }

        [[nodiscard]]
        RectF(const position_type& size)
            : pos(0, 0), size(size)
        {
        }

        [[nodiscard]]
        RectF(value_type x, value_type y, const position_type& size)
            : pos(x, y), size(size)
        {
        }

        [[nodiscard]]
        RectF(const position_type& pos, value_type width, value_type height)
            : pos(pos), size(width, height)
        {
        }

        [[nodiscard]]
        RectF(value_type x, value_type y, value_type width, value_type height)
            : pos(x, y), size(width, height)
        {
        }

        position_type tl() const
        {
            return pos;
        }

        position_type br() const
        {
            return pos + size;
        }

        position_type center() const
        {
            return pos + size / 2;
        }
    };
}
