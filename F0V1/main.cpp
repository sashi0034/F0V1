#include "pch.h"

#include "Windows.h"

#include "ZG/EngineCore.h"
#include "ZG/Logger.h"

using namespace ZG;

namespace
{
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Logger.HR().Writeln(L"application start");

    EngineCore.Init();

    Logger.HR().Writeln(L"start message loop");

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

        EngineCore.Update();
    }

    EngineCore.Destroy();

    return 0;
}
