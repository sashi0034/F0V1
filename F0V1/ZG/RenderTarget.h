#pragma once
#include "Color.h"
#include "ScopedDefer.h"
#include "Shader.h"
#include "Texture.h"
#include "Value2D.h"

namespace ZG
{
    struct RenderTargetParams
    {
        Size size;
        ColorF32 color;
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class RenderTarget
    {
    public:
        RenderTarget(const RenderTargetParams& params);

        [[nodiscard]]
        ScopedDefer scopedBind() const;

        [[nodiscard]]
        Texture texture() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
