#include "pch.h"
#include "PipelineState.h"

#include "EngineCore.h"
#include "PipelineState_impl.h"
#include "ResourceFactory.h"

namespace ZG
{
    class PipelineState::Internal
    {
    public:
        static ID3D12PipelineState* GetPipelineState(const PipelineState& pipelineState)
        {
            return pipelineState.p_impl->pipelineState;
        }

        static ID3D12RootSignature* GetRootSignature(const PipelineState& pipelineState)
        {
            return pipelineState.p_impl->rootSignature;
        }
    };

    PipelineState::PipelineState(const PipelineStateParams& params)
    {
        p_impl = EngineCore.GetResourceFactory().CreatePipelineState(params);
    }

    ScopedPipelineState::ScopedPipelineState(const PipelineState& pipelineState) :
        m_timestamp(0) // TODO
    {
        EngineCore.PushPipelineState(PipelineState::Internal::GetPipelineState(pipelineState));

        EngineCore.GetCommandList()->SetGraphicsRootSignature(PipelineState::Internal::GetRootSignature(pipelineState));
    }

    ScopedPipelineState::~ScopedPipelineState()
    {
        EngineCore.PopPipelineState();
    }
}
