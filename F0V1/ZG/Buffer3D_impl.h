#pragma once

#include <d3d12.h>

namespace ZG
{
    struct Buffer3D_impl
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    };
}
