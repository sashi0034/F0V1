#pragma once
#include <d3d12.h>

namespace ZG::detail
{
    // TODO: Obsolete?
    class EngineStackState_impl
    {
    public:
        void PushPipelineState(const ComPtr<ID3D12PipelineState>& pipelineState) const;
        void PopPipelineState() const;

        void PushRootSignature(const ComPtr<ID3D12RootSignature>& rootSignature) const;
        void PopRootSignature() const;
    };

    inline constexpr auto EngineStackState = EngineStackState_impl{};
}
