﻿#include "pch.h"
#include "Buffer3D.h"

#include "AssertObject.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
}

struct Buffer3D::Impl
{
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};

    Impl(const Buffer3DParams& params)
    {
        const auto device = EngineCore.GetDevice();

        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // GPU メモリ領域における CPU のアクセス方法
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = sizeof(params.vertexes[0]) * params.vertexes.size();
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        // アップロード
        ID3D12Resource* vertBuffer = nullptr;
        AssertWin32{"failed to create buffer"sv}
            | device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&vertBuffer)
            );

        // バッファの仮想アドレスを取得
        Vertex* vertMap{};
        AssertWin32{"failed to map vertex buffer"sv}
            | vertBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

        // マップしたメモリ位置へデータを転送
        std::ranges::copy(params.vertexes, vertMap);

        // アンマップ
        vertBuffer->Unmap(0, nullptr);

        m_vertexBufferView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = sizeof(params.vertexes[0]) * params.vertexes.size();
        m_vertexBufferView.StrideInBytes = sizeof(params.vertexes[0]);

        ID3D12Resource* indexBuffer{};
        resourceDesc.Width = sizeof(params.indices[0]) * params.indices.size();
        AssertWin32{"failed to create buffer"sv}
            | device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&indexBuffer)
            );

        // バッファの仮想アドレスを取得
        uint16_t* indexMap{};
        AssertWin32{"failed to map index buffer"sv}
            | indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));

        // マップしたメモリ位置へデータを転送
        std::ranges::copy(params.indices, indexMap);

        // アンマップ
        indexBuffer->Unmap(0, nullptr);

        m_indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.SizeInBytes = sizeof(params.indices[0]) * params.indices.size();
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    }

    void Draw() const
    {
        const auto commandList = EngineCore.GetCommandList();

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        commandList->IASetIndexBuffer(&m_indexBufferView);

        commandList->DrawIndexedInstanced(m_indexBufferView.SizeInBytes / sizeof(uint16_t), 1, 0, 0, 0);
    }
};

namespace ZG
{
    Buffer3D::Buffer3D(const Buffer3DParams& params) :
        p_impl(std::make_shared<Impl>(params))
    {
    }

    void Buffer3D::Draw() const
    {
        p_impl->Draw();
    }
}
