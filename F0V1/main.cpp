#include "Windows.h"
#include <iostream>

#include "Value2D.h"

using namespace ZG;

namespace
{
    LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
            m_windowClass.lpfnWndProc = static_cast<WNDPROC>(WindowProcedure);
            m_windowClass.lpszClassName = L"F0";
            m_windowClass.hInstance = GetModuleHandle(nullptr);
            RegisterClassEx(&m_windowClass);

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

    class EngineCore
    {
    };
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WindowCore window;
    window.Init();

    window.Show();

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
    }

    window.Destroy();

    return 0;
}
