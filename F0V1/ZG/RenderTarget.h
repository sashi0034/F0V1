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
        int bufferCount{1};
        Size size;
        ColorF32 color;
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class ScopedRenderTarget : public ScopedDefer
    {
    public:
        using ScopedDefer::ScopedDefer;
    };

    class RenderTarget
    {
    public:
        RenderTarget() = default;

        RenderTarget(const RenderTargetParams& params);

        RenderTarget(const RenderTargetParams& params, IDXGISwapChain* swapChain);

        [[nodiscard]]
        ScopedRenderTarget scopedBind(int index = 0) const;

        [[nodiscard]]
        Texture texture(int index = 0) const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
