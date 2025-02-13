#include "pch.h"
#include "PipelineState.h"

#include "AssertObject.h"
#include "EngineCore.h"
#include "PipelineState_impl.h"
#include "ScopedObjectStack.h"

using namespace ZG;

struct PipelineState::Impl
{
    ID3D12PipelineState* m_pipelineState;
    ID3D12RootSignature* m_rootSignature;

    Impl(const PipelineStateParams& params)
    {
        const auto device = EngineCore.GetDevice();
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

        {
            ID3D10Blob* rootSignatureBlob{};
            AssertWin32{"failed to create root signature"sv}
                | D3D12SerializeRootSignature(
                    &rootSignatureDesc,
                    D3D_ROOT_SIGNATURE_VERSION_1,
                    &rootSignatureBlob,
                    nullptr);
            AssertWin32{"failed to create root signature"sv}
                | device->CreateRootSignature(
                    0,
                    rootSignatureBlob->GetBufferPointer(),
                    rootSignatureBlob->GetBufferSize(),
                    IID_PPV_ARGS(&m_rootSignature));
            rootSignatureBlob->Release();
        }

        pipelineDesc.pRootSignature = m_rootSignature;

        AssertWin32{"failed to create pipeline state"sv}
            | device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pipelineState));
    }
};

struct PipelineState::Internal
{
    static void Push(const PipelineState& pipelineState)
    {
        ScopedObjectStack.PushPipelineState(pipelineState.p_impl->m_pipelineState);
        ScopedObjectStack.PushRootSignature(pipelineState.p_impl->m_rootSignature);
    }

    static void Pop()
    {
        ScopedObjectStack.PopRootSignature();
        ScopedObjectStack.PopPipelineState();
    }
};

namespace ZG
{
    PipelineState::PipelineState(const PipelineStateParams& params) :
        p_impl(std::make_shared<Impl>(params))
    {
    }

    ScopedPipelineState::ScopedPipelineState(const PipelineState& pipelineState) :
        m_timestamp(0) // TODO
    {
        PipelineState::Internal::Push(pipelineState);
    }

    ScopedPipelineState::~ScopedPipelineState()
    {
        PipelineState::Internal::Pop();
    }
}
