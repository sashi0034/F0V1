#pragma once
#include <d3d12.h>

#include "Uncopyable.h"
#include "Value2D.h"

namespace ZG
{
    struct Buffer3D_impl;
    struct Buffer3DParams;

    struct PipelineState_impl;
    struct PipelineStateParams;

    class ResourceFactory : Uncopyable
    {
    public:
        explicit ResourceFactory(ID3D12Device* device) :
            m_device(device)
        {
        }

        std::shared_ptr<Buffer3D_impl> CreateBuffer(const Buffer3DParams& params) const;

        std::shared_ptr<PipelineState_impl> CreatePipelineState(const PipelineStateParams& params) const;

    private:
        ID3D12Device* m_device;
    };
}
