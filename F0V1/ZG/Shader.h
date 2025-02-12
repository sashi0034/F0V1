#pragma once

namespace ZG
{
    struct ShaderParams
    {
        std::wstring filename;
        std::string entryPoint;
    };

    struct Shader_impl;

    class PixelShader
    {
    public:
        PixelShader(const ShaderParams& params);

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };

    class VertexShader
    {
    public:
        VertexShader(const ShaderParams& params);

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };
}
