#include "pch.h"

#include "Windows.h"

#include "EngineCore.h"
#include "Logger.h"

using namespace ZG;

namespace
{
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Logger.hr().writeln(L"application start");

    EngineCore engine{};
    engine.Init();

    Logger.hr().writeln(L"start message loop");

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

        engine.Update();
    }

    engine.Destroy();

    return 0;
}
