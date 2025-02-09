#include "Windows.h"
#include <iostream>

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
        }

    private:
        WNDCLASSEX m_windowClass{};
    };

    class EngineCore
    {
    };
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    OutputDebugString(L"Hello, World!\n");
    std::getchar();
    return 0;
}
