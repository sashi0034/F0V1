#pragma once

#include <DirectXMath.h>

namespace ZG
{
    struct Buffer3D_impl
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    };
}
