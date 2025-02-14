#pragma once

#include <d3d12.h>

namespace ZG::detail
{
    class EngineCore_impl
    {
    public:
        void Init() const;

        void BeginFrame() const;

        void EndFrame() const;

        void Destroy() const;

        ComPtr<ID3D12Device> GetDevice() const;

        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const;

        void ExecuteCommandList() const;

        ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
    };

    inline constexpr auto EngineCore = EngineCore_impl{};
}
