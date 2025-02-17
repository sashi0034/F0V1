#pragma once
#include <type_traits>

namespace ZG
{
    template <class Type>
    struct Value2D
    {
        using value_type = Type;
        // using value_type = double;

        value_type x;

        value_type y;

        [[nodiscard]] constexpr Value2D() = default;

        [[nodiscard]] constexpr Value2D(value_type _x, value_type _y) noexcept
            : x(_x), y(_y)
        {
        }

        [[nodiscard]] constexpr Value2D operator +() const noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr Value2D operator -() const noexcept
        {
            return {-x, -y};
        }

        [[nodiscard]] constexpr Value2D operator +(const Value2D& v) const noexcept
        {
            return {x + v.x, y + v.y};
        }

        [[nodiscard]] constexpr Value2D operator -(const Value2D& v) const noexcept
        {
            return {x - v.x, y - v.y};
        }

        [[nodiscard]] constexpr Value2D operator *(value_type s) const noexcept
        {
            return {x * s, y * s};
        }

        [[nodiscard]] constexpr Value2D operator /(value_type s) const noexcept
        {
            return {x / s, y / s};
        }

        template <typename T = std::conditional<std::is_floating_point_v<Type>, Type, double>::type>
        [[nodiscard]] constexpr T horizontalAspectRatio() const noexcept
        {
            return static_cast<T>(x) / static_cast<T>(y);
        }
    };

    /// @brief Floating point 2D vector
    template <class Type>
    struct Vector2D : Value2D<Type>
    {
        using Value2D<Type>::Value2D;
    };

    using Vec2 = Vector2D<double>;

    using Float2 = Vector2D<float>;

    /// @brief Integral 2D vector
    template <class Integer>
    struct Integer2D : Value2D<Integer>
    {
        using Value2D<Integer>::Value2D;
    };

    using Point = Integer2D<int32_t>;

    using Size = Point;
}
