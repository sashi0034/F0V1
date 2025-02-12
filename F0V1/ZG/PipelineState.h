#pragma once
#include "Shader.h"

namespace ZG
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

        class Internal;

    private:
        std::shared_ptr<PipelineState_impl> p_impl;
    };

    class ScopedPipelineState : Uncopyable
    {
    public:
        explicit ScopedPipelineState(const PipelineState& pipelineState);

        ~ScopedPipelineState();

    private:
        size_t m_timestamp;
    };
}
