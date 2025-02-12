#pragma once
#include "Uncopyable.h"

namespace ZG
{
    struct ShaderParams
    {
        std::wstring filename;
        std::string entryPoint;
    };

    struct Shader_impl;

    class ScopedPixelShader;

    class PixelShader
    {
    public:
        PixelShader(const ShaderParams& params);

        class Internal;

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };

    class VertexShader
    {
    public:
        VertexShader(const ShaderParams& params);

        class Internal;

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };

    class ScopedShader : Uncopyable
    {
    public:
        explicit ScopedShader(const PixelShader& pixelShader, const VertexShader& vertexShader);

        ~ScopedShader();

    private:
        size_t m_timestamp{};
    };
}
