#include "pch.h"

#include "Windows.h"
#include "ZG/Buffer3D.h"

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

    const Buffer3D buffer3D{
        Buffer3DParams{
            .vertexes = {
                {-0.4f, -0.7f, 0.0f}, //左下
                {-0.4f, 0.7f, 0.0f}, //左上
                {0.4f, -0.7f, 0.0f}, //右下
                {0.4f, 0.7f, 0.0f}, //右上
            },
            .indices = {0, 1, 2, 2, 1, 3}
        }
    };

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
