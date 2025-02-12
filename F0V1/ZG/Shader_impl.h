#pragma once
#include <d3dcommon.h>

namespace ZG
{
    struct Shader_impl
    {
        ID3DBlob* shaderBlob{};
        ID3DBlob* errorBlob{};
    };
}
