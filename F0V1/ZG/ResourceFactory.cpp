#include "pch.h"
#include "ResourceFactory.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "AssertObject.h"
#include "Buffer3D.h"
#include "Buffer3D_impl.h"
#include "Shader.h"
#include "Shader_impl.h"

namespace
{
    using namespace ZG;

    std::shared_ptr<Shader_impl> createShader(ID3D12Device* m_device, const ShaderParams& params, const char* target)
    {
        auto result = std::make_shared<Shader_impl>();

        // TODO: 例外を投げる代わりにエラーをログへ出力
        AssertWin32{"failed to create shader"sv}
            | D3DCompileFromFile(
                (params.filename).c_str(),
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                params.entryPoint.c_str(),
                target,
                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // FIXME
                0,
                &result->shaderBlob,
                &result->errorBlob
            );

        return result;
    }
}

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

    std::shared_ptr<Shader_impl> ResourceFactory::CreatePS(const ShaderParams& params) const
    {
        return createShader(m_device, params, "ps_5_0");
    }

    std::shared_ptr<Shader_impl> ResourceFactory::CreateVS(const ShaderParams& params) const
    {
        return createShader(m_device, params, "vs_5_0");
    }
}
