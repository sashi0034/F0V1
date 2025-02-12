#pragma once

#include <d3d12.h>
#include <d3dcommon.h>

#include <memory>

namespace ZG
{
    class ResourceFactory;

    class EngineCore_impl
    {
    public:
        EngineCore_impl();

        void Init() const;

        void BeginFrame() const;
        void EndFrame() const;

        void Destroy() const;

        ResourceFactory GetResourceFactory() const;

        void PushPipelineState(ID3D12PipelineState* pipelineState) const;
        void PopPipelineState() const;

        ID3D12GraphicsCommandList* GetCommandList() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    inline const auto EngineCore = EngineCore_impl{};
}
