#include "pch.h"
#include "RenderTarget.h"

#include "AssertObject.h"
#include "Texture.h"
#include "detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
}

struct RenderTarget::Impl
{
    // RenderTargetParams m_params{};
    ComPtr<ID3D12Resource> m_rtvResource{};
    ComPtr<ID3D12Resource> m_dsvResource{};

    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap{};
    ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap{};

    ColorF32 m_clearColor{};

    Texture m_texture{};

    Impl(const RenderTargetParams& params)
    {
        m_clearColor = params.color;

        const auto device = EngineCore.GetDevice();

        {
            CD3DX12_RESOURCE_DESC heapDesc{};
            heapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            heapDesc.Width = params.size.x;
            heapDesc.Height = params.size.y;
            heapDesc.DepthOrArraySize = 1;
            heapDesc.MipLevels = 1;
            heapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            heapDesc.SampleDesc = {1, 0};
            heapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            heapDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

            D3D12_CLEAR_VALUE clearValue{};
            clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            clearValue.Color[0] = m_clearColor.r;
            clearValue.Color[1] = m_clearColor.g;
            clearValue.Color[2] = m_clearColor.b;
            clearValue.Color[3] = m_clearColor.a;

            const auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

            AssertWin32{"failed to create commited resource for render target view"sv}
                | device->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &heapDesc,
                    D3D12_RESOURCE_STATE_RENDER_TARGET,
                    &clearValue,
                    IID_PPV_ARGS(&m_rtvResource));

            // -----------------------------------------------

            heapDesc.Format = DXGI_FORMAT_D32_FLOAT;
            heapDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;

            AssertWin32{"failed to create render target resource for deapth dencil buffer"sv}
                | device->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &heapDesc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    nullptr,
                    IID_PPV_ARGS(&m_dsvResource));
        }

        // -----------------------------------------------

        {
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NumDescriptors = 1;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            AssertWin32{"failed to create descriptor heap"sv}
                | device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap));

            const auto rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            device->CreateRenderTargetView(
                m_rtvResource.Get(),
                nullptr,
                rtvHandle);

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

        m_texture = Texture{
            {
                .source = m_rtvResource.Get(),
                .pixelShader = params.pixelShader,
                .vertexShader = params.vertexShader,
            }
        };
    }

    ScopedDefer ScopedBind()
    {
        const auto commandList = EngineCore.GetCommandList();

        const auto resourceBarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
            m_rtvResource.Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        commandList->ResourceBarrier(1, &resourceBarrierDesc);

        const auto rtvHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        const auto dsvHandle = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

        commandList->ClearRenderTargetView(rtvHandle, m_clearColor.getPointer(), 0, nullptr);
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        return ScopedDefer{
            [this, commandList]
            {
                const auto resourceBarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
                    m_rtvResource.Get(),
                    D3D12_RESOURCE_STATE_RENDER_TARGET,
                    D3D12_RESOURCE_STATE_PRESENT);
                commandList->ResourceBarrier(1, &resourceBarrierDesc);

                EngineCore.CommandSetDefaultRenderTargets(); // FIXME: 入れ子場の RenderTarget に対応
            }
        };
    }
};

namespace ZG
{
    RenderTarget::RenderTarget(const RenderTargetParams& params) : p_impl(std::make_shared<Impl>(params))
    {
    }

    ScopedDefer RenderTarget::scopedBind() const
    {
        return p_impl ? p_impl->ScopedBind() : ScopedDefer{};
    }

    Texture RenderTarget::texture() const
    {
        return p_impl ? p_impl->m_texture : Texture{};
    }
}
