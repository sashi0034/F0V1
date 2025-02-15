#pragma once
#include "Image.h"
#include "Shader.h"

namespace ZG
{
    class TextureBlob
    {
    public:
        TextureBlob(const std::wstring& filename);

        TextureBlob(const Image& image);

        friend class Texture;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    struct TextureParams
    {
        TextureBlob blob;
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
