#pragma once
#include "Mat4x4.h"

namespace ZG
{
    namespace Graphics3D
    {
        void SetViewMatrix(const Mat4x4& viewMatrix);

        void SetProjectionMatrix(const Mat4x4& projectionMatrix);
    }
}
