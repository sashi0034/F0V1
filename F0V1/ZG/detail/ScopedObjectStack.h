#pragma once
#include <d3d12.h>

namespace ZG::detail
{
    class EngineStackState_impl
    {
    public:
        void PushPipelineState(ID3D12PipelineState* pipelineState) const;
        void PopPipelineState() const;

        void PushRootSignature(ID3D12RootSignature* rootSignature) const;
        void PopRootSignature() const;
    };

    inline constexpr auto EngineStackState = EngineStackState_impl{};
}
