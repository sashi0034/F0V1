#pragma once
#include "Image.h"
#include "Shader.h"

namespace ZG
{
    struct TextureOptions
    {
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class Texture
    {
    public:
        Texture(std::wstring filename, const TextureOptions& options);

        Texture(const Image& image, const TextureOptions& options);

        void Draw() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
        std::wstring m_filename;
    };
}
