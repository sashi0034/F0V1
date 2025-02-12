#include "pch.h"
#include "Shader.h"

#include "EngineCore.h"
#include "ResourceFactory.h"
#include "Shader_impl.h"

using namespace ZG;

namespace
{
}

class PixelShader::Internal
{
public:
    static void Push(const PixelShader& shader)
    {
        EngineCore.PushPS(shader.p_impl->shaderBlob);
    }

    static void Pop()
    {
        EngineCore.PopPS();
    }
};

class VertexShader::Internal
{
public:
    static void Push(const VertexShader& shader)
    {
        EngineCore.PushVS(shader.p_impl->shaderBlob);
    }

    static void Pop()
    {
        EngineCore.PopVS();
    }
};

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

    ScopedShader::ScopedShader(const PixelShader& pixelShader, const VertexShader& vertexShader) :
        m_timestamp(0) // TODO
    {
        PixelShader::Internal::Push(pixelShader);
        VertexShader::Internal::Push(vertexShader);
    }

    ScopedShader::~ScopedShader()
    {
        PixelShader::Internal::Pop();
        VertexShader::Internal::Pop();
    }
}
