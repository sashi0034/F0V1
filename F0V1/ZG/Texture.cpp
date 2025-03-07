#include "pch.h"
#include "Texture.h"

#include <d3d12.h>

#include "Graphics3D.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "detail/DescriptorHeap.h"
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

    const DescriptorTable descriptorTable = {{1, 1, 0}};

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
                .descriptorTable = descriptorTable,
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

    ConstantBuffer<SceneState_b0> m_cb0{};

    DescriptorHeap m_descriptorHeap{};

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

        // -----------------------------------------------

        m_cb0 = ConstantBuffer<SceneState_b0>{1};

        m_descriptorHeap = DescriptorHeap({
            .table = descriptorTable,
            .materialCounts = {1},
            .descriptors = {CbSrUaSet{{m_cb0}, {{m_sr}}}, {}}
        });
    }

    void Draw() const
    {
        SceneState_b0 sceneState{};
        sceneState.worldMat = EngineStackState.GetWorldMatrix().mat;
        sceneState.viewMat = EngineStackState.GetViewMatrix().mat;
        sceneState.projectionMat = EngineStackState.GetProjectionMatrix().mat;
        m_cb0.upload(sceneState);

        m_pipelineState.CommandSet();

        m_descriptorHeap.CommandSet();
        m_descriptorHeap.CommandSetTable(0);

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
