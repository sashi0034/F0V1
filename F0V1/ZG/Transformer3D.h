#pragma once
#include "Mat4x4.h"
#include "Uncopyable.h"

namespace ZG
{
    class Transformer3D : Uncopyable
    {
    public:
        ASAPI_REF_CLASS(Transformer3D, asOBJ_SCOPED);

        Transformer3D() = default;

        Transformer3D(const Mat4x4& localWorldMat);
        ASAPI_CLASS_BIND(factory<const Mat4x4&>("const Mat4x4& in localWorldMat"));

        ~Transformer3D();

    private:
        bool m_active{};

        void unsafe_delete();
        ASAPI_CLASS_BIND(release(&unsafe_delete));
    };
}
