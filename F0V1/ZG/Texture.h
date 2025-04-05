#pragma once
#include "Image.h"
#include "Rect.h"
#include "Shader.h"
#include "Variant.h"

namespace ZG
{
    struct TextureParams
    {
        Variant<std::string, Image, ID3D12Resource*> source; // FIXME? 生から ComPtr はメモリリークするのだろうか
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class Texture
    {
    public:
        Texture() = default;

        Texture(const TextureParams& params);

        void draw(const RectF& region) const;

        void drawAt(const Vec2& position) const;

        void draw3D() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
        std::wstring m_filename;
    };
}
