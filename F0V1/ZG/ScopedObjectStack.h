#pragma once
#include "PipelineState_impl.h"

namespace ZG
{
    class ScopedObjectStack_impl
    {
    public:
        void PushPipelineState(ID3D12PipelineState* pipelineState) const;
        void PopPipelineState() const;

        void PushRootSignature(ID3D12RootSignature* rootSignature) const;
        void PopRootSignature() const;
    };

    inline constexpr auto ScopedObjectStack = ScopedObjectStack_impl{};
}
