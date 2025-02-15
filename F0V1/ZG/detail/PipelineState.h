#pragma once
#include "ZG/Shader.h"

namespace ZG::detail
{
    struct VertexInputElement
    {
        std::string semanticName;
        int semanticIndex;
        DXGI_FORMAT format;

        VertexInputElement() = default;

        VertexInputElement(std::string semanticName, int semanticIndex, DXGI_FORMAT format) :
            semanticName(std::move(semanticName)),
            semanticIndex(semanticIndex),
            format(format)
        {
        }
    };

    struct PipelineStateParams
    {
        PixelShader pixelShader;
        VertexShader vertexShader;
        std::vector<VertexInputElement> vertexInput;
        uint32_t srvCount{};
        uint32_t cbvCount{};
        uint32_t uavCount{};
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
