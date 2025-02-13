#include "pch.h"
#include "Shader.h"

#include <d3dcompiler.h>

#include "AssertObject.h"
#include "Logger.h"
#include "System.h"
#include "Utils.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

struct ZG::Shader_impl
{
    ID3DBlob* shaderBlob{};
    ID3DBlob* errorBlob{};

    std::string GetErrorMessage() const
    {
        if (not errorBlob) return "";
        return std::string{static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()};
    }
};

namespace
{
    std::shared_ptr<Shader_impl> createShader(const ShaderParams& params, const char* target)
    {
        auto result = std::make_shared<Shader_impl>();

        const auto compileResult = D3DCompileFromFile(
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

        if (FAILED(compileResult))
        {
            std::wstring message = L"failed to compile shader: ";
            if (compileResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
            {
                message += L"file not found";
            }
            else
            {
                message += ToUtf16(result->GetErrorMessage());
            }

            System::ModalError(message);
        }

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
