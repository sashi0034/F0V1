#pragma once
#include "Image.h"
#include "Shader.h"
#include "Variant.h"

namespace ZG
{
    struct TextureParams
    {
        Variant<std::wstring, Image> source;
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class Texture
    {
    public:
        Texture(const TextureParams& params);

        void draw() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
        std::wstring m_filename;
    };
}
