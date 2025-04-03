#pragma once

#include "Concepts.h"
#include "Value3D.h"

namespace ZG
{
    struct Mat4x4
    {
        DirectX::XMMATRIX mat;

        Mat4x4() = default;

        Mat4x4(const DirectX::XMMATRIX& xm) : mat(xm)
        {
        }

        [[nodiscard]] Mat4x4 translated(const Float3& v) const
        {
            return DirectX::XMMatrixMultiply(mat, DirectX::XMMatrixTranslation(v.x, v.y, v.z));
        }

        [[nodiscard]] Mat4x4 translated(Arithmetic auto x, Arithmetic auto y, Arithmetic auto z) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixTranslation(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
        }

        [[nodiscard]] Mat4x4 scaled(const Float3& v) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixScaling(v.x, v.y, v.z));
        }

        [[nodiscard]] Mat4x4 scaled(Arithmetic auto x, Arithmetic auto y, Arithmetic auto z) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixScaling(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)));
        }

        [[nodiscard]] Mat4x4 rotatedX(Arithmetic auto angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationX(static_cast<float>(angle)) * mat};
        }

        [[nodiscard]] Mat4x4 rotatedY(Arithmetic auto angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationY(static_cast<float>(angle)) * mat};
        }

        [[nodiscard]] Mat4x4 rotatedZ(Arithmetic auto angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationZ(static_cast<float>(angle)) * mat};
        }

        [[nodiscard]] Mat4x4 operator*(const Mat4x4& rhs) const
        {
            return Mat4x4{mat * rhs.mat};
        }

        template <typename T>
        [[nodiscard]] Vector3D<T> operator*(const Vector3D<T>& rhs) const
        {
            return Vector3D<T>{XMVector3Transform(rhs.toXMV(), mat)};
        }

        [[nodiscard]] static Mat4x4 Identity()
        {
            return Mat4x4{DirectX::XMMatrixIdentity()};
        }

        /// @brief 透視投影行列を生成する (左手座標系)
        template <typename T>
        [[nodiscard]] static Mat4x4 LookAt(const Vector3D<T>& eye, const Vector3D<T>& target, const Vector3D<T>& up)
        {
            return Mat4x4{DirectX::XMMatrixLookAtLH(eye.toXMV(), target.toXMV(), up.toXMV())};
        }

        [[nodiscard]] static Mat4x4 PerspectiveFov(
            Arithmetic auto fov, Arithmetic auto aspect, Arithmetic auto nearZ, Arithmetic auto farZ)
        {
            return Mat4x4{
                DirectX::XMMatrixPerspectiveFovLH(
                    static_cast<float>(fov),
                    static_cast<float>(aspect),
                    static_cast<float>(nearZ),
                    static_cast<float>(farZ))
            };
        }

        [[nodiscard]] static Mat4x4 Translate(const Float3 v) noexcept
        {
            return DirectX::XMMatrixTranslation(v.x, v.y, v.z);
        }
    };
}
