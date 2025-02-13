#pragma once

#include <d3d12.h>
#include <d3dcommon.h>

#include <memory>

namespace ZG::detail
{
    class EngineCore_impl
    {
    public:
        void Init() const;

        void BeginFrame() const;

        void EndFrame() const;

        void Destroy() const;

        ID3D12Device* GetDevice() const;

        ID3D12GraphicsCommandList* GetCommandList() const;
    };

    inline constexpr auto EngineCore = EngineCore_impl{};
}
