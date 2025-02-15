#pragma once
#include "ZG/Shader.h"

namespace ZG::detail
{
    struct PipelineStateParams
    {
        PixelShader pixelShader;
        VertexShader vertexShader;
    };

    struct PipelineState_impl;

    class PipelineState
    {
    public:
        PipelineState(const PipelineStateParams& params);

        void CommandSet() const;

        struct Internal;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    // class ScopedPipelineState : Uncopyable
    // {
    // public:
    //     explicit ScopedPipelineState(const PipelineState& pipelineState);
    //
    //     ~ScopedPipelineState();
    //
    // private:
    //     size_t m_timestamp;
    // };
}
