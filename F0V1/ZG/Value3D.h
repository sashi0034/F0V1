#pragma once
#include "TemplateHelper.h"
#include "Script/Script_asapi.h"

namespace ZG
{
    template <class Type>
    struct Value3D
    {
        static constexpr auto objectProperty =
            asOBJ_POD |
            (std::is_floating_point_v<Type> ? asOBJ_APP_CLASS_ALLFLOATS : asOBJ_APP_CLASS_ALLINTS) |
            asOBJ_APP_CLASS_MORE_CONSTRUCTORS;

        ASAPI_VALUE_CLASS_AS("$Value3D", Value3D, objectProperty);

        using value_type = Type;
        // using value_type = double;

        value_type x;
        ASAPI_CLASS_PROPERTY("$value_type x", x);

        value_type y;
        ASAPI_CLASS_PROPERTY("$value_type y", y);

        value_type z;
        ASAPI_CLASS_PROPERTY("$value_type z", z);

        static constexpr bool isFloat3 = std::is_same_v<value_type, float>;

        static constexpr bool isVec3 = std::is_same_v<value_type, double>;

        ASAPI_MACRO_PREPROCESSOR(
            if constexpr (isFloat3) macro({{"$Value3D", "Float3"}, {"$value_type", "float"}});
            else if constexpr (isVec3) macro({{"$Value3D", "Vec3"}, {"$value_type", "double"}});
            else static_assert(always_false<Type>);
        );

        Value3D() = default;

        Value3D(value_type x, value_type y, value_type z) : x(x), y(y), z(z)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <value_type, value_type, value_type>
            (t("$value_type x, $value_type y, $value_type z")));

        template <typename OtherType> requires std::is_convertible_v<OtherType, value_type>
        [[nodiscard]] Value3D(const Value3D<OtherType>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
        {
        }

        ASAPI_CLASS_CONSTRUCTOR_WHEN(
            not isFloat3,
            <const Value3D<float>&>
            (t("const Float3& in other")));

        ASAPI_CLASS_CONSTRUCTOR_WHEN(
            not isVec3,
            <const Value3D<double>&>
            (t("const Vec3& in other")));

        Value3D(DirectX::XMFLOAT3 xmf) : x(xmf.x), y(xmf.y), z(xmf.z)
        {
        }

        Value3D(DirectX::XMVECTOR xmv)
        {
            DirectX::XMFLOAT3 tmp;
            XMStoreFloat3(&tmp, xmv);
            *this = tmp;
        }

        [[nodiscard]] constexpr Value3D operator+(const Value3D& rhs) const
        {
            return Value3D(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        [[nodiscard]] constexpr Value3D operator-(const Value3D& rhs) const
        {
            return Value3D(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        ASAPI_CLASS_OPERATOR(_this - const_this);

        [[nodiscard]] constexpr Value3D operator*(value_type rhs) const
        {
            return Value3D(x * rhs, y * rhs, z * rhs);
        }

        ASAPI_CLASS_OPERATOR(_this * param<value_type>);

        [[nodiscard]] constexpr Value3D operator/(value_type rhs) const
        {
            return Value3D(x / rhs, y / rhs, z / rhs);
        }

        ASAPI_CLASS_OPERATOR(_this / param<value_type>);

        [[nodiscard]] constexpr bool operator==(const Value3D& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        ASAPI_CLASS_BIND(opEquals());

        [[nodiscard]] constexpr bool operator!=(const Value3D& rhs) const
        {
            return !(*this == rhs);
        }

        [[nodiscard]] DirectX::XMFLOAT3 toXMF() const
        {
            if constexpr (std::is_same_v<value_type, float>) return {x, y, z};
            else return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)};
        }

        [[nodiscard]] DirectX::XMVECTOR toXMV() const
        {
            const auto tmp = toXMF();
            return DirectX::XMLoadFloat3(&tmp);
        }
    };

    /// @brief Floating point 3D vector
    template <class Type>
    struct Vector3D : Value3D<Type>
    {
        using value_type = Type;

        using Value3D<Type>::Value3D;

        [[nodiscard]] constexpr value_type dot(const Vector3D& rhs) const
        {
            return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
        }

        [[nodiscard]] constexpr Vector3D cross(const Vector3D& rhs) const
        {
            return Vector3D(
                this->y * rhs.z - this->z * rhs.y,
                this->z * rhs.x - this->x * rhs.z,
                this->x * rhs.y - this->y * rhs.x);
        }

        [[nodiscard]] value_type length() const
        {
            return std::sqrt(this->dot(*this));
        }

        [[nodiscard]] constexpr Vector3D normalized() const
        {
            return *this / length();
        }
    };

    using Vec3 = Vector3D<double>;

    using Float3 = Vector3D<float>;

    template <typename T>
    concept FloatingPoint3D = std::is_base_of_v<Vector3D<T>, T>;
}
