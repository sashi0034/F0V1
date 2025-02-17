#pragma once
#include "Shader.h"

namespace ZG
{
    struct ModelParams
    {
        std::string filename;
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    class Model
    {
    public:
        Model(const ModelParams& params);

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
