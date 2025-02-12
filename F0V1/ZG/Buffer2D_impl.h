#pragma once
#include "IResource.h"

#include <DirectXMath.h>

namespace ZG
{
    struct Buffer2DState : IResourceState
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
        D3D12_INDEX_BUFFER_VIEW indexBufferView{};
    };

    struct Buffer2DProps : IResourceProps
    {
        std::vector<DirectX::XMFLOAT3> vertexes;
        std::vector<uint16_t> indices;
    };
}
