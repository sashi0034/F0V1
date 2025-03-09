#include "pch.h"
#include "EngineCore.h"

#include <cassert>

#include "Windows.h"

#include "ZG/Value2D.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "CommandList.h"
#include "EngineHotReloader.h"
#include "EnginePresetAsset.h"
#include "EngineWindow.h"
#include "ZG/Array.h"
#include "ZG/AssertObject.h"
#include "ZG/Color.h"
#include "ZG/EngineTimer.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

namespace
{
    using namespace ZG;
    using namespace ZG::detail;

    using namespace std::string_view_literals;

    void enableDebugLayer()
    {
        ID3D12Debug* debugLayer = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
        {
            debugLayer->EnableDebugLayer();
            debugLayer->Release();
        }
    }

    constexpr ColorF32 defaultClearColor = {0.5f, 0.5f, 0.5f, 1.0f};

    constexpr Size defaultSceneSize = {1280, 720};

    struct Impl
    {
        Point m_sceneSize{defaultSceneSize};
        ColorF32 m_clearColor{defaultClearColor};

        ID3D12Device* m_device{};
        IDXGIFactory6* m_dxgiFactory{};
        IDXGIAdapter* m_adapter{};
        D3D_FEATURE_LEVEL m_featureLevel{};

        // FIXME: グローバルオブジェクトは ComPtr にしなくていいかも

        CommandList m_commandList{};
        CommandList m_copyCommandList{};

        ComPtr<IDXGISwapChain4> m_swapChain{};
        ComPtr<ID3D12DescriptorHeap> m_rtvHeaps{};

        ComPtr<ID3D12Resource> m_depthBuffer{};
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap{};

        D3D12_RESOURCE_BARRIER m_barrierDesc{};

        std::vector<ComPtr<ID3D12Resource>> m_backBuffers{};

        Array<std::weak_ptr<IEngineUpdatable>> m_updatableList{};

        void Init()
        {
            EngineWindow.Init();
#ifdef _DEBUG
            enableDebugLayer();
#endif

            // デバッグフラグ有効で DXGI ファクトリを生成
            if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory))))
            {
                // 失敗した場合、デバッグフラグ無効で DXGI ファクトリを生成
                if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
                {
                    throw std::runtime_error("failed to create DXGI Factory");
                }
            }

            // 利用可能なアダプタを取得
            std::vector<IDXGIAdapter*> availableAdapters{};
            {
                IDXGIAdapter* tmp = nullptr;
                for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmp) != DXGI_ERROR_NOT_FOUND; ++i)
                {
                    availableAdapters.push_back(tmp);
                }
            }

            // 最適なアダプタを選択
            for (const auto adapter : availableAdapters)
            {
                DXGI_ADAPTER_DESC desc = {};
                adapter->GetDesc(&desc);
                std::wstring strDesc = desc.Description;
                if (strDesc.find(L"NVIDIA") != std::string::npos)
                {
                    m_adapter = adapter;
                    break;
                }
            }

            if (not m_adapter)
            {
                throw std::runtime_error("failed to select adapter");
            }

            // Direct3D デバイスの初期化
            static constexpr std::array levels = {
                D3D_FEATURE_LEVEL_12_1,
                D3D_FEATURE_LEVEL_12_0,
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
            };

            for (const auto level : levels)
            {
                if (D3D12CreateDevice(m_adapter, level, IID_PPV_ARGS(&m_device)) == S_OK)
                {
                    m_featureLevel = level;
                    break;
                }
            }

            // コマンドリストの作成
            m_commandList = CommandList{CommandListType::Direct};

            m_copyCommandList = CommandList{CommandListType::Copy};

            // スワップチェインの設定
            DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
            swapchainDesc.Width = m_sceneSize.x;
            swapchainDesc.Height = m_sceneSize.y;
            swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapchainDesc.Stereo = false;
            swapchainDesc.SampleDesc.Count = 1;
            swapchainDesc.SampleDesc.Quality = 0;
            swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
            swapchainDesc.BufferCount = 2;
            swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            AssertWin32{"failed to create swap chain"sv}
                | m_dxgiFactory->CreateSwapChainForHwnd(
                    m_commandList.GetCommandQueue(),
                    EngineWindow.Handle(),
                    &swapchainDesc,
                    nullptr,
                    nullptr,
                    reinterpret_cast<IDXGISwapChain1**>(m_swapChain.GetAddressOf())
                );

            // ディスクリプタヒープを生成
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            heapDesc.NodeMask = 0;
            heapDesc.NumDescriptors = 2; // 表裏の２つ
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            AssertWin32{"failed to create descriptor heap"sv}
                | m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeaps));

            // AssertWin32{"failed to get swap chain description"sv}
            //     | m_sapChain->GetDesc(&swapchainDesc);

            m_backBuffers.resize(swapchainDesc.BufferCount);
            D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
            for (size_t i = 0; i < swapchainDesc.BufferCount; ++i)
            {
                AssertWin32{"failed to get buffer"sv}
                    | m_swapChain->GetBuffer(static_cast<UINT>(i), IID_PPV_ARGS(&m_backBuffers[i]));
                m_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, handle);
                handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            }

            // 深度バッファを生成
            createDepthBuffer();

            // ウィンドウ表示
            EngineWindow.Show();

            // タイマーの初期化
            EngineTimer.Reset();

            // プリセットの初期化
            EnginePresetAsset.Init();
        }

        void BeginFrame()
        {
            const auto commandList = m_commandList.GetCommandList();

            // バックバッファのインデックスを取得
            const auto backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

            // リソースバリア
            m_barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            m_barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            m_barrierDesc.Transition.pResource = m_backBuffers[backBufferIndex].Get();
            m_barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            commandList->ResourceBarrier(1, &m_barrierDesc);

            // レンダーターゲットを指定
            auto rtvHandle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
            rtvHandle.ptr += static_cast<ULONG_PTR>(
                backBufferIndex * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
            const auto dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
            commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

            // 画面クリア
            commandList->ClearRenderTargetView(rtvHandle, m_clearColor.getPointer(), 0, nullptr);
            commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

            // ビューポートの設定
            Point windowSize = EngineWindow.WindowSize();
            D3D12_VIEWPORT viewport = {};
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;
            viewport.Width = static_cast<float>(windowSize.x);
            viewport.Height = static_cast<float>(windowSize.y);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            commandList->RSSetViewports(1, &viewport);

            // シザー矩形の設定
            D3D12_RECT scissorRect = {};
            scissorRect.left = 0;
            scissorRect.top = 0;
            scissorRect.right = scissorRect.left + windowSize.x;
            scissorRect.bottom = scissorRect.top + windowSize.y;
            commandList->RSSetScissorRects(1, &scissorRect);

            // タイマーの更新
            EngineTimer.Tick();

            // アップデータの更新
            for (auto& updatable : m_updatableList)
            {
                if (const auto updatablePtr = updatable.lock())
                {
                    updatablePtr->Update();
                }
            }

            // ホットリローダの更新
            EngineHotReloader.Update();
        }

        void EndFrame()
        {
            // リソースバリア
            m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            m_commandList.GetCommandList()->ResourceBarrier(1, &m_barrierDesc);

            // コマンドリストのクローズ
            m_commandList.GetCommandList()->Close();

            // コマンドリストの実行
            m_commandList.Flush();

            // フリップ
            m_swapChain->Present(1, 0);
        }

        void Destroy()
        {
            EngineHotReloader.Destroy();

            EnginePresetAsset.Destroy();

            EngineWindow.Destroy();
        }

    private:
        void createDepthBuffer()
        {
            D3D12_RESOURCE_DESC desc{};
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            desc.Alignment = 0;
            desc.Width = m_sceneSize.x;
            desc.Height = m_sceneSize.y;
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_D32_FLOAT;
            desc.SampleDesc = {1, 0};
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            D3D12_HEAP_PROPERTIES heapProperties{};
            heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            D3D12_CLEAR_VALUE clearValue{};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.0f; // 深度値 1.0 でクリア

            AssertWin32{"failed to create depth buffer"sv}
                | m_device->CreateCommittedResource(
                    &heapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &desc,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE,
                    &clearValue,
                    IID_PPV_ARGS(&m_depthBuffer)
                );

            D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // デプスステンシルビュー
            heapDesc.NumDescriptors = 1;

            AssertWin32{"failed to create descriptor heap"sv}
                | m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_dsvHeap));

            // デプスステンシルビューの作成
            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            m_device->CreateDepthStencilView(
                m_depthBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
        }
    } s_engineCore{};
}

namespace ZG
{
    void EngineCore_impl::Init() const
    {
        s_engineCore.Init();
    }

    void EngineCore_impl::BeginFrame() const
    {
        s_engineCore.BeginFrame();
    }

    void EngineCore_impl::EndFrame() const
    {
        s_engineCore.EndFrame();
    }

    void EngineCore_impl::Destroy() const
    {
        s_engineCore.Destroy();
    }

    ID3D12Device* EngineCore_impl::GetDevice() const
    {
        assert(s_engineCore.m_device);
        return s_engineCore.m_device;
    }

    ID3D12GraphicsCommandList* EngineCore_impl::GetCommandList() const
    {
        assert(s_engineCore.m_commandList.GetCommandList());
        return s_engineCore.m_commandList.GetCommandList();
    }

    ID3D12GraphicsCommandList* EngineCore_impl::GetCopyCommandList() const
    {
        assert(s_engineCore.m_copyCommandList.GetCommandList());
        return s_engineCore.m_copyCommandList.GetCommandList();
    }

    void EngineCore_impl::FlushCopyCommandList() const
    {
        s_engineCore.m_copyCommandList.Flush();
    }

    Size EngineCore_impl::GetSceneSize() const
    {
        return s_engineCore.m_sceneSize;
    }

    void EngineCore_impl::AddUpdatable(const std::weak_ptr<IEngineUpdatable>& updatable) const
    {
        s_engineCore.m_updatableList.push_back(updatable);
    }
}
