#pragma once

#include "Value3D.h"

namespace ZG
{
    struct alignas(16) Mat4x4
    {
        ASAPI_REF_CLASS(Mat4x4, asOBJ_NOCOUNT);

        DirectX::XMMATRIX mat;

        Mat4x4() = default;

        Mat4x4(const DirectX::XMMATRIX& xm) : mat(xm)
        {
        }

        [[nodiscard]] Mat4x4 translated(const Float3& v) const
        {
            return DirectX::XMMatrixMultiply(mat, DirectX::XMMatrixTranslation(v.x, v.y, v.z));
        }

        [[nodiscard]] Mat4x4 translated(float x, float y, float z) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixTranslation(x, y, z));
        }

        [[nodiscard]] Mat4x4 scaled(const Float3& v) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixScaling(v.x, v.y, v.z));
        }

        [[nodiscard]] Mat4x4 scaled(float x, float y, float z) const
        {
            return DirectX::XMMatrixMultiply(
                mat, DirectX::XMMatrixScaling(x, y, z));
        }

        [[nodiscard]] Mat4x4 rotatedX(float angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationX(angle) * mat};
        }

        [[nodiscard]] Mat4x4 rotatedY(float angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationY(angle) * mat};
        }

        [[nodiscard]] Mat4x4 rotatedZ(float angle) const
        {
            return Mat4x4{DirectX::XMMatrixRotationZ(angle) * mat};
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
            float fov, float aspect, float nearZ, float farZ)
        {
            return Mat4x4{DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ)};
        }

        [[nodiscard]] static Mat4x4 Translate(const Float3 v) noexcept
        {
            return DirectX::XMMatrixTranslation(v.x, v.y, v.z);
        }
    };

    /// @brief スクリプト用のラッパクラス。AngelScript では 16-byte alignment が保証されないため、参照型を用いる
    struct Mat4x4Ref
    {
        ASAPI_VALUE_CLASS(Mat4x4Ref, asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        std::shared_ptr<Mat4x4> ptr = std::make_shared<Mat4x4>();

        Mat4x4Ref() = default;

        Mat4x4Ref(const std::shared_ptr<Mat4x4>& p) : ptr(p)
        {
        }

        Mat4x4Ref(const Mat4x4& m) : ptr(std::make_shared<Mat4x4>(m))
        {
        }

        [[nodiscard]] Mat4x4Ref clone() const
        {
            return Mat4x4Ref(std::make_shared<Mat4x4>(*ptr));
        }

        ASAPI_CLASS_METHOD("Mat4x4Ref clone() const", clone);

        [[nodiscard]] Mat4x4Ref translate(const Float3& v)
        {
            *ptr = ptr->translated(v);
            return *this;
        }

        ASAPI_CLASS_METHOD_BY(
            "Mat4x4Ref translate(const Float3& in v)",
            <const Float3&>(&Mat4x4Ref::translate));

        [[nodiscard]] Mat4x4Ref translate(float x, float y, float z)
        {
            *ptr = ptr->translated(x, y, z);
            return *this;
        }

        ASAPI_CLASS_METHOD_BY(
            "Mat4x4Ref translate(float x, float y, float z)",
            <float, float, float>(&Mat4x4Ref::translate));

        [[nodiscard]] Mat4x4Ref scale(const Float3& v) const
        {
            *ptr = ptr->scaled(v);
            return *this;
        }

        ASAPI_CLASS_METHOD_BY(
            "Mat4x4Ref scale(const Float3& in v)",
            <const Float3&>(&Mat4x4Ref::scale, const_));

        [[nodiscard]] Mat4x4Ref scale(float x, float y, float z)
        {
            *ptr = ptr->scaled(x, y, z);
            return *this;
        }

        ASAPI_CLASS_METHOD_BY(
            "Mat4x4Ref scale(float x, float y, float z)",
            <float, float, float>(&Mat4x4Ref::scale));

        [[nodiscard]] Mat4x4Ref rotateX(float angle)
        {
            *ptr = ptr->rotatedX(angle);
            return *this;
        }

        ASAPI_CLASS_METHOD("Mat4x4Ref rotateX(float angle)", rotateX);

        [[nodiscard]] Mat4x4Ref rotateY(float angle)
        {
            *ptr = ptr->rotatedY(angle);
            return *this;
        }

        ASAPI_CLASS_METHOD("Mat4x4Ref rotateY(float angle)", rotateY);

        [[nodiscard]] Mat4x4Ref rotateZ(float angle)
        {
            *ptr = ptr->rotatedZ(angle);
            return *this;
        }

        ASAPI_CLASS_METHOD("Mat4x4Ref rotateZ(float angle)", rotateZ);

        [[nodiscard]] Mat4x4Ref operator*(const Mat4x4Ref& rhs)
        {
            *ptr = (*ptr) * (*rhs.ptr);
            return *this;
        }

        ASAPI_CLASS_BIND(use(_this * const_this));

        [[nodiscard]] const Mat4x4& get()
        {
            return *ptr;
        }

        ASAPI_CLASS_BIND(
            method("const Mat4x4& get()", overload_cast<>(&Mat4x4Ref::get))
            .method("const Mat4x4& opImplConv()", overload_cast<>(&Mat4x4Ref::get))
        );

        [[nodiscard]] const Mat4x4& get() const
        {
            return *ptr;
        }

        ASAPI_CLASS_BIND(
            method("Mat4x4& get() const", overload_cast<>(&Mat4x4Ref::get, const_))
            .method("Mat4x4& opImplConv() const", overload_cast<>(&Mat4x4Ref::get, const_))
        )
    };

    namespace Mat4x4Ref_
    {
        ASAPI_NAMESPACE("Mat4x4Ref");

        [[nodiscard]] static Mat4x4Ref Identity()
        {
            return {Mat4x4::Identity()};
        }

        ASAPI_GLOBAL_FUNCTION("Mat4x4Ref Identity()", &Identity);

        [[nodiscard]] static Mat4x4Ref LookAt(const Float3& eye, const Float3& target, const Float3& up)
        {
            return {Mat4x4::LookAt(eye, target, up)};
        }

        ASAPI_GLOBAL_FUNCTION(
            "Mat4x4Ref LookAt(const Float3& in eye, const Float3& in target, const Float3& in up)",
            &LookAt);

        [[nodiscard]] static Mat4x4Ref PerspectiveFov(float fov, float aspect, float nearZ, float farZ)
        {
            return {Mat4x4::PerspectiveFov(fov, aspect, nearZ, farZ)};
        }

        ASAPI_GLOBAL_FUNCTION(
            "Mat4x4Ref PerspectiveFov(float fov, float aspect, float nearZ, float farZ)",
            &PerspectiveFov);

        [[nodiscard]] static Mat4x4Ref Translate(const Float3& v)
        {
            return {Mat4x4::Translate(v)};
        }

        ASAPI_GLOBAL_FUNCTION("Mat4x4Ref Translate(const Float3& in v)", &Translate);
    }
}
