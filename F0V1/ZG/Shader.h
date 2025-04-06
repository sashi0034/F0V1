#pragma once
#include <d3dcommon.h>

#include "ITimestamp.h"
#include "Script/Script_asapi.h"

namespace ZG
{
    struct ShaderParams
    {
        std::string filename;
        std::string entryPoint;
    };

    struct Shader_impl;

    class ScopedPixelShader;

    class PixelShader
    {
    public:
        ASAPI_VALUE_CLASS(PixelShader, asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        PixelShader() = default;

        explicit PixelShader(const ShaderParams& params);

        explicit PixelShader(const std::string& filename, const std::string& entryPoint)
            : PixelShader{ShaderParams{.filename = filename, .entryPoint = entryPoint}}
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const std::string&, const std::string&>
            ( t("const string& in filename, const string& in entryPoint = 'PS'")));

        [[nodiscard]] bool isEmpty() const;
        ASAPI_CLASS_METHOD("bool isEmpty() const", isEmpty);

        [[nodiscard]] std::shared_ptr<ITimestamp> timestamp() const;

        [[nodiscard]] ID3D10Blob* getBlob() const;

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };

    class VertexShader
    {
    public:
        ASAPI_VALUE_CLASS(VertexShader, asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        VertexShader() = default;

        explicit VertexShader(const ShaderParams& params);

        explicit VertexShader(const std::string& filename, const std::string& entryPoint)
            : VertexShader{ShaderParams{.filename = filename, .entryPoint = entryPoint}}
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const std::string&, const std::string&>
            ( t("const string& in filename, const string& in entryPoint = 'VS'")));

        [[nodiscard]] bool isEmpty() const;
        ASAPI_CLASS_METHOD("bool isEmpty() const", isEmpty);

        [[nodiscard]] std::shared_ptr<ITimestamp> timestamp() const;

        [[nodiscard]] ID3D10Blob* getBlob() const;

    private:
        std::shared_ptr<Shader_impl> p_impl;
    };

    // class ScopedShader : Uncopyable
    // {
    // public:
    //     explicit ScopedShader(const PixelShader& pixelShader, const VertexShader& vertexShader);
    //
    //     ~ScopedShader();
    //
    // private:
    //     size_t m_timestamp{};
    // };
}
