#include "pch.h"
#include "ResourceFactory.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "AssertObject.h"
#include "Buffer3D.h"
#include "Buffer3D_impl.h"
#include "PipelineState.h"
#include "PipelineState_impl.h"
#include "Shader_impl.h"

namespace
{
    using namespace ZG;
}

namespace ZG
{
    std::shared_ptr<Buffer3D_impl> ResourceFactory::CreateBuffer(const Buffer3DParams& params) const
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

    std::shared_ptr<PipelineState_impl> ResourceFactory::CreatePipelineState(const PipelineStateParams& params) const
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};

        pipelineDesc.VS.pShaderBytecode = params.vertexShader.GetBlob()->GetBufferPointer();
        pipelineDesc.VS.BytecodeLength = params.vertexShader.GetBlob()->GetBufferSize();

        pipelineDesc.PS.pShaderBytecode = params.pixelShader.GetBlob()->GetBufferPointer();
        pipelineDesc.PS.BytecodeLength = params.pixelShader.GetBlob()->GetBufferSize();

        pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 0xffffffff

        pipelineDesc.BlendState.AlphaToCoverageEnable = false;
        pipelineDesc.BlendState.IndependentBlendEnable = false;

        D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
        renderTargetBlendDesc.BlendEnable = false;
        renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        renderTargetBlendDesc.LogicOpEnable = false;

        pipelineDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

        pipelineDesc.RasterizerState.MultisampleEnable = false;
        pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        pipelineDesc.RasterizerState.DepthClipEnable = true;

        pipelineDesc.RasterizerState.FrontCounterClockwise = false;
        pipelineDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        pipelineDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        pipelineDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        pipelineDesc.RasterizerState.AntialiasedLineEnable = false;
        pipelineDesc.RasterizerState.ForcedSampleCount = 0;
        pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        pipelineDesc.DepthStencilState.DepthEnable = false;
        pipelineDesc.DepthStencilState.StencilEnable = false;

        // TODO: input layout
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            {
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                D3D12_APPEND_ALIGNED_ELEMENT,
                D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
                0
            },
        };

        pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
        pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

        pipelineDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; // ストリップ時のカットなし
        pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 三角形で構成

        pipelineDesc.NumRenderTargets = 1;
        pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        pipelineDesc.SampleDesc.Count = 1; // マルチサンプリングなし
        pipelineDesc.SampleDesc.Quality = 0; // クオリティ最低

        // TODO: root signature
        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ID3D12RootSignature* rootSignature{};

        {
            ID3D10Blob* rootSignatureBlob{};
            AssertWin32{"failed to create root signature"sv}
                | D3D12SerializeRootSignature(
                    &rootSignatureDesc,
                    D3D_ROOT_SIGNATURE_VERSION_1,
                    &rootSignatureBlob,
                    nullptr);
            AssertWin32{"failed to create root signature"sv}
                | m_device->CreateRootSignature(
                    0,
                    rootSignatureBlob->GetBufferPointer(),
                    rootSignatureBlob->GetBufferSize(),
                    IID_PPV_ARGS(&rootSignature));
            rootSignatureBlob->Release();
        }

        pipelineDesc.pRootSignature = rootSignature;

        ID3D12PipelineState* pipelineState = nullptr;
        AssertWin32{"failed to create pipeline state"sv}
            | m_device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
        return std::make_shared<PipelineState_impl>(pipelineState, rootSignature);
    }
}
