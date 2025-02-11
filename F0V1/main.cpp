#include <array>

#include "Windows.h"
#include <iostream>

#include "Value2D.h"

#include<d3d12.h>
#include<dxgi1_6.h>
#include <format>
#include <iso646.h>
#include <vector>
#include <string_view>

#include "AssertObject.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace ZG;

namespace
{
    using namespace std::string_view_literals;

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

            m_windowSize = {1280, 720};

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

    class EngineCore
    {
    public:
        void Init()
        {
            // デバッグフラグ有効で DXGI ファクトリを生成
            if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory))))
            {
                // 失敗した場合、デバッグフラグ無効で DXGI ファクトリを生成
                if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_dxgiFactory))))
                {
                    throw std::runtime_error("Failed to create DXGI Factory");
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
                throw std::runtime_error("Failed to select adapter");
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
        }

    private:
        ID3D12Device* m_device{};
        IDXGIFactory6* m_dxgiFactory{};
        IDXGISwapChain4* m_sapChain{};
        IDXGIAdapter* m_adapter{};
        D3D_FEATURE_LEVEL m_featureLevel{};
        ID3D12CommandAllocator* m_commandAllocator{};
        ID3D12GraphicsCommandList* m_commandList{};
    };
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    OutputDebugString(L"--- application start\n");

    WindowCore window{};
    window.Init();

#ifdef _DEBUG
    enableDebugLayer();
#endif

    EngineCore engine{};
    engine.Init();

    window.Show();

    OutputDebugString(L"--- start message loop\n");

    while (true)
    {
        MSG msg;
        if (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }

        // -----------------------------------------------
    }

    window.Destroy();

    return 0;
}
