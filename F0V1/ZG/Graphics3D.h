#pragma once
#include "Mat4x4.h"
#include "Script/Script_asapi.h"

namespace ZG
{
    class VertexBuffer_impl;
    class IndexBuffer;

    namespace Graphics3D
    {
        ASAPI_NAMESPACE("Graphics3D");

        void SetViewMatrix(const Mat4x4& viewMatrix);
        ASAPI_GLOBAL_FUNCTION("void SetViewMatrix(const Mat4x4& in viewMatrix)", SetViewMatrix);

        void SetProjectionMatrix(const Mat4x4& projectionMatrix);
        ASAPI_GLOBAL_FUNCTION("void SetProjectionMatrix(const Mat4x4& in projectionMatrix)", SetProjectionMatrix);

        void DrawTriangles(const VertexBuffer_impl& vertexBuffer, const IndexBuffer& indexBuffer);
    }
}
