#pragma once
#include "Rect.h"
#include "Shader.h"
#include "TextureSource.h"

namespace ZG
{
    struct TextureParams
    {
        TextureSource source; // FIXME? 生から ComPtr はメモリリークするのだろうか
        PixelShader ps;
        VertexShader vs;
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
