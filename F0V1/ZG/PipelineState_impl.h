#pragma once
#include <d3d12.h>

namespace ZG
{
    struct PipelineState_impl
    {
        ID3D12PipelineState* pipelineState;
        ID3D12RootSignature* rootSignature;
    };
}
