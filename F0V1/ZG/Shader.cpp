#include "pch.h"
#include "Shader.h"

#include <d3dcompiler.h>

#include "AssertObject.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

struct ZG::Shader_impl
{
    ID3DBlob* shaderBlob{};
    ID3DBlob* errorBlob{};
};

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
}
