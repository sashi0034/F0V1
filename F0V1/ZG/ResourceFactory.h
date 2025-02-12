#pragma once
#include <d3d12.h>

#include "Uncopyable.h"

namespace ZG
{
    struct Buffer3D_impl;
    struct Buffer3DParams;

    struct Shader_impl;
    struct ShaderParams;

    class ResourceFactory : Uncopyable
    {
    public:
        explicit ResourceFactory(ID3D12Device* device) :
            m_device(device)
        {
        }

        std::shared_ptr<Buffer3D_impl> Create(const Buffer3DParams& params) const;

    private:
        ID3D12Device* m_device;
    };
}
