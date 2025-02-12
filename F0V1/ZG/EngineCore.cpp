#include "pch.h"
#include "EngineCore.h"

#include "Windows.h"

#include "Value2D.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#include "AssertObject.h"
#include "Buffer3D_impl.h"
#include "ColorF32.h"
#include "ResourceFactory.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

namespace
{
    using namespace ZG;

    using namespace std::string_view_literals;

    constexpr Point DefaultWindowSize{1280, 720};

    LRESULT windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (msg == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    class WindowCore
    {
    public:
        void Init()
        {
            m_windowClass.cbSize = sizeof(WNDCLASSEX);
            m_windowClass.lpfnWndProc = static_cast<WNDPROC>(windowProcedure);
            m_windowClass.lpszClassName = L"F0";
            m_windowClass.hInstance = GetModuleHandle(nullptr);
            RegisterClassEx(&m_windowClass);

            // -----------------------------------------------

            m_windowSize = DefaultWindowSize;

            RECT windowRect{0, 0, m_windowSize.x, m_windowSize.y};
            AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

            m_handle = CreateWindow(
                m_windowClass.lpszClassName,
                L"F0",
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                nullptr, // hWndParent
                nullptr, // hMenu 
                m_windowClass.hInstance, // hInstance, 
                nullptr // lpParam
            );
        }

        void Show()
        {
            ShowWindow(m_handle, SW_SHOW);
        }

        void Destroy()
        {
            UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
        }

        Point WindowSize() const { return m_windowSize; }

        HWND Handle() const { return m_handle; }

    private:
        WNDCLASSEX m_windowClass{};
        Point m_windowSize{};
        HWND m_handle{};
    };

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
}

struct EngineCore_impl::Impl
{
public:
    void Init()
    {
        m_window.Init();
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
                m_commandAllocator,
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
        swapchainDesc.Width = DefaultWindowSize.x;
        swapchainDesc.Height = DefaultWindowSize.y;
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
                m_commandQueue,
                m_window.Handle(),
                &swapchainDesc,
                nullptr,
                nullptr,
                reinterpret_cast<IDXGISwapChain1**>(&m_swapChain)
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
            m_device->CreateRenderTargetView(m_backBuffers[i], nullptr, handle);
            handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // フェンスを生成
        AssertWin32{"failed to create fence"sv}
            | m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

        // ウィンドウ表示
        m_window.Show();
    }

    void Update()
    {
        // バックバッファのインデックスを取得
        const auto backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        // リソースバリア
        D3D12_RESOURCE_BARRIER barrierDesc{};
        barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrierDesc.Transition.pResource = m_backBuffers[backBufferIndex];
        barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        m_commandList->ResourceBarrier(1, &barrierDesc);

        // レンダーターゲットを指定
        auto rtvHandle = m_rtvHeaps->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += static_cast<ULONG_PTR>(
            backBufferIndex * m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        m_commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

        // 画面クリア
        m_commandList->ClearRenderTargetView(rtvHandle, m_clearColor.GetPointer(), 0, nullptr);

        barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        m_commandList->ResourceBarrier(1, &barrierDesc);

        // コマンドリストのクローズ
        m_commandList->Close();

        // コマンドリストの実行
        ID3D12CommandList* commandLists[] = {m_commandList};
        m_commandQueue->ExecuteCommandLists(1, commandLists);

        // 実行の待機
        m_fenceValue++;
        m_commandQueue->Signal(m_fence, m_fenceValue);

        if (m_fence->GetCompletedValue() != m_fenceValue)
        {
            m_fence->SetEventOnCompletion(m_fenceValue, nullptr);
            WaitForSingleObjectEx(nullptr, INFINITE, false);
        }

        // コマンドアロケータのリセット
        m_commandAllocator->Reset();

        // コマンドリストのリセット
        m_commandList->Reset(m_commandAllocator, nullptr);

        // フリップ
        m_swapChain->Present(1, 0);
    }

    void Destroy()
    {
        m_window.Destroy();
    }

    ResourceFactory GetResourceFactory() const
    {
        AssertNotNull{"the engine is not initialized"sv}
            | m_device;
        return ResourceFactory{m_device};
    }

    WindowCore m_window{};

    ColorF32 m_clearColor{defaultClearColor};

    ID3D12Device* m_device{};
    IDXGIFactory6* m_dxgiFactory{};
    IDXGIAdapter* m_adapter{};
    D3D_FEATURE_LEVEL m_featureLevel{};

    ID3D12CommandAllocator* m_commandAllocator{};
    ID3D12GraphicsCommandList* m_commandList{};
    ID3D12CommandQueue* m_commandQueue{};
    IDXGISwapChain4* m_swapChain{};
    ID3D12DescriptorHeap* m_rtvHeaps{};

    ID3D12Fence* m_fence{};
    UINT64 m_fenceValue{};

    std::vector<ID3D12Resource*> m_backBuffers{};

    std::vector<ID3DBlob*> m_psBlobs{};
    std::vector<ID3DBlob*> m_vsBlobs{};
};

namespace ZG
{
    EngineCore_impl::EngineCore_impl() :
        p_impl(std::make_shared<Impl>())
    {
    }

    void EngineCore_impl::Init() const
    {
        p_impl->Init();
    }

    void EngineCore_impl::Update() const
    {
        p_impl->Update();
    }

    void EngineCore_impl::Destroy() const
    {
        p_impl->Destroy();
    }

    ResourceFactory EngineCore_impl::GetResourceFactory() const
    {
        return p_impl->GetResourceFactory();
    }

    void EngineCore_impl::PushPS(ID3DBlob* psBlob) const
    {
        assert(psBlob);
        p_impl->m_psBlobs.push_back(psBlob);
    }

    void EngineCore_impl::PopPS() const
    {
        assert(not p_impl->m_psBlobs.empty());
        p_impl->m_psBlobs.pop_back();
    }

    void EngineCore_impl::PushVS(ID3DBlob* vsBlob) const
    {
        assert(vsBlob);
        p_impl->m_vsBlobs.push_back(vsBlob);
    }

    void EngineCore_impl::PopVS() const
    {
        assert(not p_impl->m_vsBlobs.empty());
        p_impl->m_vsBlobs.pop_back();
    }
}
