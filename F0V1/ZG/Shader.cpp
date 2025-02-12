﻿#include "pch.h"
#include "Shader.h"

#include <d3dcompiler.h>

#include "AssertObject.h"
#include "EngineCore.h"
#include "Shader_impl.h"

using namespace ZG;

namespace
{
    std::shared_ptr<Shader_impl> createShader(const ShaderParams& params, const char* target)
    {
        auto result = std::make_shared<Shader_impl>();

        // TODO: 例外を投げる代わりにエラーをログへ出力
        AssertWin32{"failed to create shader"sv}
            | D3DCompileFromFile(
                (params.filename).c_str(),
                nullptr,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                params.entryPoint.c_str(),
                target,
                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // FIXME
                0,
                &result->shaderBlob,
                &result->errorBlob
            );

        return result;
    }
}

namespace ZG
{
    PixelShader::PixelShader(const ShaderParams& params)
    {
        p_impl = createShader(params, "ps_5_0");
    }

    ID3D10Blob* PixelShader::GetBlob() const
    {
        return p_impl->shaderBlob;
    }

    VertexShader::VertexShader(const ShaderParams& params)
    {
        p_impl = createShader(params, "vs_5_0");
    }

    ID3D10Blob* VertexShader::GetBlob() const
    {
        return p_impl->shaderBlob;
    }

    // ScopedShader::ScopedShader(const PixelShader& pixelShader, const VertexShader& vertexShader) :
    //     m_timestamp(0) // TODO
    // {
    //     PixelShader::Internal::Push(pixelShader);
    //     VertexShader::Internal::Push(vertexShader);
    // }
    //
    // ScopedShader::~ScopedShader()
    // {
    //     PixelShader::Internal::Pop();
    //     VertexShader::Internal::Pop();
    // }
}
