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
        ASAPI_VALUE_CLASS(Texture, asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        Texture() = default;

        Texture(const TextureParams& params);

        Texture(const TextureSource& source, const PixelShader& ps, const VertexShader& vs)
            : Texture(TextureParams{source, ps, vs})
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <TextureSource, PixelShader, VertexShader>
            ("const TextureSource& in source, const PixelShader& in ps, const VertexShader& in vs"));

        void draw(const RectF& region) const;
        ASAPI_CLASS_METHOD("void draw(const RectF& in region) const", draw);

        void drawAt(const Vec2& position) const;
        ASAPI_CLASS_METHOD("void drawAt(const Vec2& in position) const", drawAt);

        void draw3D() const;
        ASAPI_CLASS_METHOD("void draw3D() const", draw3D);

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
        std::wstring m_filename;
    };
}
