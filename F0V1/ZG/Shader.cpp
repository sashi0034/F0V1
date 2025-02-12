#include "pch.h"
#include "Shader.h"

#include "EngineCore.h"
#include "ResourceFactory.h"

namespace ZG
{
    PixelShader::PixelShader(const ShaderParams& params)
    {
        p_impl = EngineCore.GetResourceFactory().CreatePS(params);
    }

    VertexShader::VertexShader(const ShaderParams& params)
    {
        p_impl = EngineCore.GetResourceFactory().CreateVS(params);
    }
}
