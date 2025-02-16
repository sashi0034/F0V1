#include "pch.h"
#include "Graphics3D.h"

#include "detail/EngineStackState.h"

namespace ZG
{
    using namespace detail;

    void Graphics3D::SetViewMatrix(const Mat4x4& viewMatrix)
    {
        EngineStackState.SetViewMatrix(viewMatrix);
    }

    void Graphics3D::SetProjectionMatrix(const Mat4x4& projectionMatrix)
    {
        EngineStackState.SetProjectionMatrix(projectionMatrix);
    }
}
