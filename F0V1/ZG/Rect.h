#pragma once
#include "Value2D.h"

namespace ZG
{
    template <typename Type>
    struct Rectangle
    {
        ASAPI_VALUE_CLASS_AS("$Rectangle", Rectangle<Type>, asOBJ_POD | asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        using value_type = Type;
        using position_type = Value2D<value_type>;

        position_type pos;
        position_type size;

        ASAPI_MACRO_PREPROCESSOR(
            if constexpr (std::is_same_v<Type, double>)
            { macro({{"$Rectangle", "RectF"}, {"$value_type", "double"}, {"$position_type", "Vec2"}}); }
            else static_assert(always_false<Type>);
        );

        [[nodiscard]]
        Rectangle() = default;

        [[nodiscard]]
        Rectangle(const position_type& pos, const position_type& size)
            : pos(pos), size(size)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const position_type&, const position_type&>
            (t("const $position_type& in pos, const $position_type& in size")));

        [[nodiscard]]
        Rectangle(const position_type& size)
            : pos(0, 0), size(size)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const position_type&>
            (t("const $position_type& in size")));

        [[nodiscard]]
        Rectangle(value_type x, value_type y, const position_type& size)
            : pos(x, y), size(size)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <value_type, value_type, const position_type&>
            (t("$value_type x, $value_type y, const $position_type& in size")));

        [[nodiscard]]
        Rectangle(const position_type& pos, value_type width, value_type height)
            : pos(pos), size(width, height)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const position_type&, value_type, value_type>
            (t("const $position_type& in pos, $value_type width, $value_type height")));

        [[nodiscard]]
        Rectangle(value_type x, value_type y, value_type width, value_type height)
            : pos(x, y), size(width, height)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <value_type, value_type, value_type, value_type>
            (t("$value_type x, $value_type y, $value_type width, $value_type height")));

        position_type tl() const
        {
            return pos;
        }

        ASAPI_CLASS_METHOD("$position_type tl() const", tl);

        position_type br() const
        {
            return pos + size;
        }

        ASAPI_CLASS_METHOD("$position_type br() const", br);

        position_type center() const
        {
            return pos + size / 2;
        }

        ASAPI_CLASS_METHOD("$position_type center() const", center);
    };

    using RectF = Rectangle<double>;
}
