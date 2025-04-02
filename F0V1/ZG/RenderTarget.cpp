#include "pch.h"
#include "RenderTarget.h"

#include "AssertObject.h"
#include "Texture.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
    struct ImplBase
    {
        D3D12_CPU_DESCRIPTOR_HANDLE m_lastRtvHandle{};
        D3D12_CPU_DESCRIPTOR_HANDLE m_lastDsvHandle{};
    };

    Array<ImplBase*> s_renderTargetStack{};
}

struct RenderTarget::Impl : ImplBase
{
    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap{};
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap{};

    std::vector<ComPtr<ID3D12Resource>> m_rtvResources{};
    ComPtr<ID3D12Resource> m_dsvResource{};

    // RenderTargetParams m_params{};

    ColorF32 m_clearColor{};

    std::vector<Texture> m_textures{};

    Impl(const RenderTargetParams& params, IDXGISwapChain* swapChain = nullptr)
    {
        m_clearColor = params.color;

        const auto device = EngineCore.GetDevice();

        if (not swapChain)
        {
            // 通常のレンダーターゲット
            CD3DX12_RESOURCE_DESC resourceDesc{};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = params.size.x;
            resourceDesc.Height = params.size.y;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            resourceDesc.SampleDesc = {1, 0};
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            D3D12_CLEAR_VALUE clearValue{};
            clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            clearValue.Color[0] = m_clearColor.r;
            clearValue.Color[1] = m_clearColor.g;
            clearValue.Color[2] = m_clearColor.b;
            clearValue.Color[3] = m_clearColor.a;

            const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

            m_rtvResources.resize(params.bufferCount);
            for (int i = 0; i < params.bufferCount; ++i)
            {
                AssertWin32{"failed to create commited resource for render target view"sv}
                    | device->CreateCommittedResource(
                        &heapProperties,
                        D3D12_HEAP_FLAG_NONE,
                        &resourceDesc,
                        D3D12_RESOURCE_STATE_RENDER_TARGET,
                        &clearValue,
                        IID_PPV_ARGS(&m_rtvResources[i]));
            }
        }
        else // バックバッファ
        {
            m_rtvResources.resize(params.bufferCount);
            for (int i = 0; i < params.bufferCount; ++i)
            {
                AssertWin32{"failed to get buffer"sv}
                    | swapChain->GetBuffer(i, IID_PPV_ARGS(&m_rtvResources[i]));
            }
        }

        // -----------------------------------------------

        {
            CD3DX12_RESOURCE_DESC resourceDesc{};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            resourceDesc.Width = params.size.x;
            resourceDesc.Height = params.size.y;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
            resourceDesc.SampleDesc = {1, 0};
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

            const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

            D3D12_CLEAR_VALUE clearValue{};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.0f;
            clearValue.DepthStencil.Stencil = 0;

            AssertWin32{"failed to create render target resource for deapth dencil buffer"sv}
                | device->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &resourceDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &clearValue,
                    IID_PPV_ARGS(&m_dsvResource));
        }

        // -----------------------------------------------

        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NumDescriptors = params.bufferCount;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            AssertWin32{"failed to create descriptor heap"sv}
                | device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap));

            auto rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

            for (int i = 0; i < params.bufferCount; ++i)
            {
                device->CreateRenderTargetView(
                    m_rtvResources[i].Get(),
                    nullptr,
                    rtvHandle);

                rtvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            // -----------------------------------------------

            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            AssertWin32{"failed to create descriptor heap"sv}
                | device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvDescriptorHeap));

            const auto dsvHandle = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            device->CreateDepthStencilView(
                m_dsvResource.Get(),
                nullptr,
                dsvHandle);
        }

        // -----------------------------------------------

        if (not swapChain)
        {
            // FIXME?
            m_textures.resize(params.bufferCount);
            for (int i = 0; i < params.bufferCount; ++i)
            {
                m_textures[i] = Texture{
                    {
                        .source = m_rtvResources[i].Get(),
                        .pixelShader = params.pixelShader,
                        .vertexShader = params.vertexShader,
                    }
                };
            }
        }
    }

    ScopedRenderTarget ScopedBind(int index)
    {
        s_renderTargetStack.push_back(this);

        const auto commandList = EngineCore.GetCommandList();

        const auto resourceBarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
            m_rtvResources[index].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &resourceBarrierDesc);

        auto rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr +=
            index * EngineCore.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        const auto dsvHandle = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

        commandList->ClearRenderTargetView(rtvHandle, m_clearColor.getPointer(), 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        m_lastRtvHandle = rtvHandle;
        m_lastDsvHandle = dsvHandle;

        return ScopedRenderTarget{
            [this, index, commandList]
            {
                const auto resourceBarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
                    m_rtvResources[index].Get(),
                    D3D12_RESOURCE_STATE_RENDER_TARGET,
                    D3D12_RESOURCE_STATE_PRESENT);
                commandList->ResourceBarrier(1, &resourceBarrierDesc);

                AssertTrue{"RenderTarget::ScopedBind() is not balanced"sv}
                    | s_renderTargetStack[s_renderTargetStack.size() - 1] == this;
                s_renderTargetStack.pop_back();

                if (s_renderTargetStack.empty()) return;
                // -----------------------------------------------

                const auto prev = s_renderTargetStack[s_renderTargetStack.size() - 1];
                commandList->OMSetRenderTargets(1, &prev->m_lastRtvHandle, false, &prev->m_lastDsvHandle);
            }
        };
    }
};

namespace ZG
{
    RenderTarget::RenderTarget(const RenderTargetParams& params)
        : p_impl(std::make_shared<Impl>(params))
    {
    }

    RenderTarget::RenderTarget(const RenderTargetParams& params, IDXGISwapChain* swapChain)
        : p_impl(std::make_shared<Impl>(params, swapChain))
    {
    }

    ScopedRenderTarget RenderTarget::scopedBind(int index) const
    {
        return p_impl ? p_impl->ScopedBind(index) : ScopedRenderTarget{};
    }

    Texture RenderTarget::texture(int index) const
    {
        return p_impl ? p_impl->m_textures[index] : Texture{};
    }
}
