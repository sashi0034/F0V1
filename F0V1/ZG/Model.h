#pragma once
#include "Array.h"
#include "ConstantBuffer.h"
#include "Shader.h"

namespace ZG
{
    struct ModelParams
    {
        std::string filename;
        PixelShader ps;
        VertexShader vs;
        std::optional<ConstantBuffer_impl> cb2{};
    };

    class Model
    {
    public:
        ASAPI_VALUE_CLASS(Model, asOBJ_APP_CLASS_MORE_CONSTRUCTORS);

        Model() = default;

        Model(const ModelParams& params);

        Model(const std::string& filename, const PixelShader& ps, const VertexShader& vs)
            : Model{ModelParams{filename, ps, vs}}
        {
        }

        ASAPI_CLASS_CONSTRUCTOR(
            <const std::string&, const PixelShader&, const VertexShader&>
            (t("const string& in filename, const PixelShader& in ps, const VertexShader& in vs")));

        void draw() const;
        ASAPI_CLASS_METHOD("void draw() const", draw);

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
