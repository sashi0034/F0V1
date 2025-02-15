#include "pch.h"
#include "EngineCore.h"

#include <cassert>

#include "Windows.h"

#include "ZG/Value2D.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "EngineWindow.h"
#include "ZG/AssertObject.h"
#include "ZG/Color.h"

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
    public:
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

            // コマンドアロケータを生成
            AssertWin32{"failed to create command allocator"sv}
                | m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

            // コマンドリストを生成
            AssertWin32{"failed to create command list"sv}
                | m_device->CreateCommandList(
                    0,
                    D3D12_COMMAND_LIST_TYPE_DIRECT,
                    m_commandAllocator.Get(),
                    nullptr,
                    IID_PPV_ARGS(&m_commandList)
                );

            // コマンドキューを生成
            D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
            commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE; // タイムアウトなし
            commandQueueDesc.NodeMask = 0;
            commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL; // プライオリティ特に指定なし
            commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            AssertWin32{"failed to create command queue"sv}
                | m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue));

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
                    m_commandQueue.Get(),
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

            // フェンスを生成
            AssertWin32{"failed to create fence"sv}
                | m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

            // ウィンドウ表示
            EngineWindow.Show();
        }

        void BeginFrame()
        {
            // バックバッファのインデックスを取得
            const auto backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

            // リソースバリア
            m_barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            m_barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            m_barrierDesc.Transition.pResource = m_backBuffers[backBufferIndex].Get();
            m_barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
            m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
            m_commandList->ResourceBarrier(1, &m_barrierDesc);

            // レンダーターゲットを指定
            auto rtvHandle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
            rtvHandle.ptr += static_cast<ULONG_PTR>(
                backBufferIndex * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
            m_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

            // 画面クリア
            m_commandList->ClearRenderTargetView(rtvHandle, m_clearColor.getPointer(), 0, nullptr);

            // ビューポートの設定
            Point windowSize = EngineWindow.WindowSize();
            D3D12_VIEWPORT viewport = {};
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;
            viewport.Width = static_cast<float>(windowSize.x);
            viewport.Height = static_cast<float>(windowSize.y);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            m_commandList->RSSetViewports(1, &viewport);

            // シザー矩形の設定
            D3D12_RECT scissorRect = {};
            scissorRect.left = 0;
            scissorRect.top = 0;
            scissorRect.right = scissorRect.left + windowSize.x;
            scissorRect.bottom = scissorRect.top + windowSize.y;
            m_commandList->RSSetScissorRects(1, &scissorRect);
        }

        void ExecuteCommandList()
        {
            ID3D12CommandList* commandLists[] = {m_commandList.Get()};
            m_commandQueue->ExecuteCommandLists(1, commandLists);

            // 実行の待機
            m_fenceValue++;
            m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

            if (m_fence->GetCompletedValue() != m_fenceValue)
            {
                const auto event = CreateEvent(nullptr, false, false, nullptr);
                m_fence->SetEventOnCompletion(m_fenceValue, event);
                WaitForSingleObjectEx(event, INFINITE, false);
                CloseHandle(event);
            }
        }

        void EndFrame()
        {
            // リソースバリア
            m_barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
            m_barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
            m_commandList->ResourceBarrier(1, &m_barrierDesc);

            // コマンドリストのクローズ
            m_commandList->Close();

            // コマンドリストの実行
            ExecuteCommandList();

            // コマンドアロケータのリセット
            m_commandAllocator->Reset();

            // コマンドリストのリセット
            m_commandList->Reset(m_commandAllocator.Get(), nullptr);

            // フリップ
            m_swapChain->Present(1, 0);
        }

        void Destroy()
        {
            EngineWindow.Destroy();
        }

        Point m_sceneSize{defaultSceneSize};
        ColorF32 m_clearColor{defaultClearColor};

        ID3D12Device* m_device{};
        IDXGIFactory6* m_dxgiFactory{};
        IDXGIAdapter* m_adapter{};
        D3D_FEATURE_LEVEL m_featureLevel{};

        // FIXME: グローバルオブジェクトは ComPtr にしなくていいかも

        ComPtr<ID3D12CommandAllocator> m_commandAllocator{};
        ComPtr<ID3D12GraphicsCommandList> m_commandList{};
        ComPtr<ID3D12CommandQueue> m_commandQueue{};
        ComPtr<IDXGISwapChain4> m_swapChain{};
        ComPtr<ID3D12DescriptorHeap> m_rtvHeaps{};

        ComPtr<ID3D12Fence> m_fence{};
        UINT64 m_fenceValue{};

        D3D12_RESOURCE_BARRIER m_barrierDesc{};

        std::vector<ComPtr<ID3D12Resource>> m_backBuffers{};
    } s_impl{};
}

namespace ZG
{
    void EngineCore_impl::Init() const
    {
        s_impl.Init();
    }

    void EngineCore_impl::BeginFrame() const
    {
        s_impl.BeginFrame();
    }

    void EngineCore_impl::EndFrame() const
    {
        s_impl.EndFrame();
    }

    void EngineCore_impl::Destroy() const
    {
        s_impl.Destroy();
    }

    ComPtr<ID3D12Device> EngineCore_impl::GetDevice() const
    {
        assert(s_impl.m_device);
        return s_impl.m_device;
    }

    ComPtr<ID3D12GraphicsCommandList> EngineCore_impl::GetCommandList() const
    {
        assert(s_impl.m_commandList);
        return s_impl.m_commandList;
    }

    void EngineCore_impl::ExecuteCommandList() const
    {
        s_impl.ExecuteCommandList();
    }

    ComPtr<ID3D12CommandQueue> EngineCore_impl::GetCommandQueue() const
    {
        assert(s_impl.m_commandQueue);
        return s_impl.m_commandQueue;
    }

    Size EngineCore_impl::GetSceneSize() const
    {
        return s_impl.m_sceneSize;
    }
}
