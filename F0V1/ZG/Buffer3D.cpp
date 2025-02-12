#include "pch.h"
#include "Buffer3D.h"

#include "Buffer3D_impl.h"
#include "EngineCore.h"
#include "ResourceFactory.h"

namespace ZG
{
    Buffer3D::Buffer3D(const Buffer3DParams& params)
    {
        p_impl = EngineCore.GetResourceFactory().CreateBuffer(params);
    }

    void Buffer3D::Draw() const
    {
        const auto commandList = EngineCore.GetCommandList();

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        commandList->IASetVertexBuffers(0, 1, &p_impl->vertexBufferView);
        commandList->IASetIndexBuffer(&p_impl->indexBufferView);

        commandList->DrawIndexedInstanced(p_impl->indexBufferView.SizeInBytes / sizeof(uint16_t), 1, 0, 0, 0);
    }
}
