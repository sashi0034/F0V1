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

struct ZG::Shader_impl : ITimestamp
{
    uint64_t m_timestamp{};
    ComPtr<ID3DBlob> shaderBlob{};
    ComPtr<ID3DBlob> errorBlob{};

    std::string GetErrorMessage() const
    {
        if (not errorBlob) return "";
        return std::string{static_cast<char*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()};
    }

    uint64_t timestamp() override
    {
        return m_timestamp;
    }
};

namespace
{
    std::shared_ptr<Shader_impl> createShader(const ShaderParams& params, const char* target)
    {
        auto result = std::make_shared<Shader_impl>();
        result->m_timestamp = System::FrameCount();

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

        if (SUCCEEDED(compileResult)) return result;
        // -----------------------------------------------

        std::wstring message = L"failed to compile shader: ";
        if (compileResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            message += L"file not found";
        }
        else
        {
            message += ToUtf16(result->GetErrorMessage());
        }

        LogError.Writeln(message);
        return nullptr;
    }
}

namespace ZG
{
    PixelShader::PixelShader(const ShaderParams& params)
    {
        p_impl = createShader(params, "ps_5_0");
    }

    bool PixelShader::isEmpty() const
    {
        return p_impl == nullptr;
    }

    std::shared_ptr<ITimestamp> PixelShader::timestamp() const
    {
        return p_impl;
    }

    ID3D10Blob* PixelShader::getBlob() const
    {
        return p_impl ? p_impl->shaderBlob.Get() : nullptr;
    }

    VertexShader::VertexShader(const ShaderParams& params)
    {
        p_impl = createShader(params, "vs_5_0");
    }

    bool VertexShader::isEmpty() const
    {
        return p_impl == nullptr;
    }

    ID3D10Blob* VertexShader::getBlob() const
    {
        return p_impl ? p_impl->shaderBlob.Get() : nullptr;
    }
}
