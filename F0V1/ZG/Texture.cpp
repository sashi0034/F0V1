#include "pch.h"
#include "Texture.h"

#include <d3d12.h>

#include "AssertObject.h"
#include "Graphics3D.h"
#include "IndexBuffer.h"
#include "System.h"
#include "Utils.h"
#include "VertexBuffer.h"
#include "detail/EngineCore.h"
#include "detail/EngineStackState.h"
#include "detail/PipelineState.h"
#include "detail/ShaderResourceTexture.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
    struct TextureVertex
    {
        Float3 position;
        Float2 uv;
    };

    VertexBuffer<TextureVertex> makeVertexBuffer()
    {
        return Array<TextureVertex>{
            {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //左下
            {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, //左上
            {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, //右下
            {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, //右上
        };
    }

    IndexBuffer makeIndexBuffer()
    {
        return Array<uint16_t>{0, 1, 2, 2, 1, 3};
    }

    PipelineState makePipelineState(const TextureParams& options)
    {
        // TODO: キャッシュする?
        return PipelineState{
            PipelineStateParams{
                .pixelShader = options.pixelShader,
                .vertexShader = options.vertexShader,
                .vertexInput = {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT}
                },
                .descriptorTable = {{1, 1, 0}},
            }
        };
    }

    struct SceneState_b0
    {
        Mat4x4 worldMat;
        Mat4x4 viewMat;
        Mat4x4 projectionMat;
    };
}

struct Texture::Impl
{
    ShaderResourceTexture m_sr;

    PipelineState m_pipelineState;

    VertexBuffer<TextureVertex> m_vertexBuffer{makeVertexBuffer()};
    IndexBuffer m_indexBuffer{makeIndexBuffer()};

    ComPtr<ID3D12Resource> m_constantBuffer{};

    SceneState_b0* m_mappedCB0{};

    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap{};

    Impl(const TextureParams& options) :
        m_pipelineState(makePipelineState(options))
    {
        if (const auto path = options.source.tryGet<std::wstring>())
        {
            m_sr = ShaderResourceTexture{*path};
        }
        else if (const auto image = options.source.tryGet<Image>())
        {
            m_sr = ShaderResourceTexture{*image};
        }
        else
        {
            assert(false);
        }

        createDescriptorHeap(m_sr.getFormat());
    }

    // TODO: 行列の更新をできるようにする

    void createDescriptorHeap(DXGI_FORMAT format)
    {
        // 定数バッファ作成
        using namespace DirectX;

        const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignedSize(sizeof(SceneState_b0), 256));
        AssertWin32{"failed to create commited resource"sv}
            | EngineCore.GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_constantBuffer));

        AssertWin32{"failed to map constant buffer"sv}
            | m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedCB0));
        // TODO: Unmap?

        // ディスクリプタヒープの作成
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descriptorHeapDesc.NodeMask = 0;
        descriptorHeapDesc.NumDescriptors = 2; // SRV と CBV の2つ
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        AssertWin32{"failed to create descriptor heap"sv}
            | EngineCore.GetDevice()->CreateDescriptorHeap(
                &descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));

        // テクスチャビュー (SRV) 作成
        auto basicHeapHandle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;

        EngineCore.GetDevice()->CreateShaderResourceView(
            m_sr.getResource(), &srvDesc, basicHeapHandle);

        // CBV 作成
        basicHeapHandle.ptr += EngineCore.GetDevice()->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(m_constantBuffer->GetDesc().Width);

        EngineCore.GetDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
    }

    void Draw() const
    {
        SceneState_b0 sceneState{};
        sceneState.worldMat = EngineStackState.GetWorldMatrix().mat;
        sceneState.viewMat = EngineStackState.GetViewMatrix().mat;
        sceneState.projectionMat = EngineStackState.GetProjectionMatrix().mat;
        *m_mappedCB0 = sceneState;

        m_pipelineState.CommandSet();

        const auto commandList = EngineCore.GetCommandList();
        commandList->SetDescriptorHeaps(
            1, m_descriptorHeap.GetAddressOf());
        commandList->SetGraphicsRootDescriptorTable(
            0, m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());

        Graphics3D::DrawTriangles(m_vertexBuffer, m_indexBuffer);
    }
};

namespace ZG
{
    Texture::Texture(const TextureParams& params) :
        p_impl{std::make_shared<Impl>(params)}
    {
    }

    void Texture::draw() const
    {
        p_impl->Draw();
    }
}
