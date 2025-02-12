#include "pch.h"
#include "Buffer3D.h"

#include "EngineCore.h"
#include "ResourceFactory.h"

namespace ZG
{
    Buffer3D::Buffer3D(const Buffer3DParams& params)
    {
        p_impl = EngineCore.GetResourceFactory().Create(params);
    }
}
