﻿#pragma once
#include <type_traits>

#include "TemplateHelper.h"
#include "Script/Script_asapi.h"

namespace ZG
{
    template <class Type>
    struct Value2D
    {
        static constexpr auto objectProperty =
            asOBJ_POD |
            (std::is_floating_point_v<Type> ? asOBJ_APP_CLASS_ALLFLOATS : asOBJ_APP_CLASS_ALLINTS) |
            asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

        ASAPI_VALUE_CLASS_AS("$Value2D", Value2D, objectProperty);

        using value_type = Type;
        // using value_type = double;

        value_type x;
        ASAPI_CLASS_PROPERTY("$value_type x", x);

        value_type y;
        ASAPI_CLASS_PROPERTY("$value_type y", y);

        static constexpr bool isPoint = std::is_same_v<value_type, int>;

        static constexpr bool isFloat2 = std::is_same_v<value_type, float>;

        static constexpr bool isVec2 = std::is_same_v<value_type, double>;

        ASAPI_MACRO_PREPROCESSOR(
            if constexpr (isPoint) macro({{"$Value2D", "Point"}, {"$value_type", "int"}});
            else if constexpr (isFloat2) macro({{"$Value2D", "Float2"}, {"$value_type", "float"}});
            else if constexpr (isVec2) macro({{"$Value2D", "Vec2"}, {"$value_type", "double"}});
            else static_assert(always_false<Type>);
        );

        [[nodiscard]] constexpr Value2D() = default;

        [[nodiscard]] constexpr Value2D(value_type _x, value_type _y) noexcept
            : x(_x), y(_y)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <value_type, value_type>
            (t("$value_type x, $value_type y")));

        template <typename OtherType>
        [[nodiscard]] constexpr Value2D(const Value2D<OtherType>& other) noexcept
            : x(other.x), y(other.y)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR_WHEN(
            not isPoint,
            <const Value2D<int>&>
            (t("const Point& in other")));

        ASAPI_CLASS_CONSTRUCTOR_WHEN(
            not isFloat2,
            <const Value2D<float>&>
            (t("const Float2& in other")));

        ASAPI_CLASS_CONSTRUCTOR_WHEN(
            not isVec2,
            <const Value2D<double>&>
            (t("const Vec2& in other")));

        [[nodiscard]] constexpr Value2D operator +() const noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr Value2D operator -() const noexcept
        {
            return {-x, -y};
        }

        ASAPI_CLASS_OPERATOR(-_this);

        [[nodiscard]] constexpr Value2D operator +(const Value2D& v) const noexcept
        {
            return {x + v.x, y + v.y};
        }

        ASAPI_CLASS_OPERATOR(_this + const_this);

        [[nodiscard]] constexpr Value2D operator -(Value2D v) const noexcept
        {
            return {x - v.x, y - v.y};
        }

        ASAPI_CLASS_OPERATOR(_this - const_this);

        [[nodiscard]] constexpr Value2D operator *(value_type s) const noexcept
        {
            return {x * s, y * s};
        }

        ASAPI_CLASS_OPERATOR(_this * param<value_type>);

        [[nodiscard]] constexpr Value2D operator /(value_type s) const noexcept
        {
            return {x / s, y / s};
        }

        ASAPI_CLASS_OPERATOR(_this / param<value_type>);

        [[nodiscard]] constexpr Value2D withX(value_type newX) const noexcept
        {
            return {newX, y};
        }

        ASAPI_CLASS_METHOD("$Value2D withX($value_type newX) const", withX);

        [[nodiscard]] constexpr Value2D withY(value_type newY) const noexcept
        {
            return {x, newY};
        }

        ASAPI_CLASS_METHOD("$Value2D withY($value_type newY) const", withX);

        [[nodiscard]] constexpr Value2D<int> toPoint() const noexcept
        {
            return {static_cast<int>(x), static_cast<int>(y)};
        }

        template <class OtherType>
        [[nodiscard]] constexpr Value2D<OtherType> cast() const noexcept
        {
            return {static_cast<OtherType>(x), static_cast<OtherType>(y)};
        }

        template <typename T = std::conditional<std::is_floating_point_v<Type>, Type, double>::type>
        [[nodiscard]] constexpr T horizontalAspectRatio() const noexcept
        {
            return static_cast<T>(x) / static_cast<T>(y);
        }

        ASAPI_CLASS_BIND(method("double horizontalAspectRatio() const", &horizontalAspectRatio<double>));
    };

    /// @brief Floating point 2D vector
    // template <class Type>
    // struct Vector2D : Value2D<Type>
    // {
    //     using Value2D<Type>::Value2D;
    //
    //     [[nodiscard]] constexpr Vector2D(const Value2D<Type>& v)
    //         : Value2D<Type>(v)
    //     {
    //     }
    //
    //     template <class OtherType>
    //     [[nodiscard]] constexpr Vector2D(const Value2D<OtherType>& v)
    //         : Value2D<Type>(static_cast<Type>(v.x), static_cast<Type>(v.y))
    //     {
    //     }
    // };

    using Vec2 = Value2D<double>;

    using Float2 = Value2D<float>;

    /// @brief Integral 2D vector
    // template <class Integer>
    // struct Integer2D : Value2D<Integer>
    // {
    //     using Value2D<Integer>::Value2D;
    // };

    using Point = Value2D<int32_t>;

    using Size = Point;
}
