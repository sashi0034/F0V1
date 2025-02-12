#include "pch.h"
#include "ResourceFactory.h"

#include "AssertObject.h"
#include "Buffer3D.h"
#include "Buffer3D_impl.h"

namespace ZG
{
    std::shared_ptr<Buffer3D_impl> ResourceFactory::Create(const Buffer3DParams& params) const
    {
        D3D12_HEAP_PROPERTIES heapProperties = {};
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // GPU メモリ領域における CPU のアクセス方法
        heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = sizeof(params.vertexes);
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
            | m_device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&vertBuffer)
            );

        // バッファの仮想アドレスを取得
        DirectX::XMFLOAT3* vertMap{};
        AssertWin32{"failed to map vertex buffer"sv}
            | vertBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));

        // マップしたメモリ位置へデータを転送
        std::ranges::copy(params.vertexes, vertMap);

        // アンマップ
        vertBuffer->Unmap(0, nullptr);

        Buffer3D_impl result{};
        result.vertexBufferView.BufferLocation = vertBuffer->GetGPUVirtualAddress();
        result.vertexBufferView.SizeInBytes = sizeof(params.vertexes);
        result.vertexBufferView.StrideInBytes = sizeof(params.vertexes[0]);

        ID3D12Resource* indexBuffer{};
        resourceDesc.Width = sizeof(params.indices);
        AssertWin32{"failed to create buffer"sv}
            | m_device->CreateCommittedResource(
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

        result.indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
        result.indexBufferView.SizeInBytes = sizeof(params.indices);
        result.indexBufferView.Format = DXGI_FORMAT_R16_UINT;

        return std::make_shared<Buffer3D_impl>(result);
    }
}
