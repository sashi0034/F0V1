#include "pch.h"
#include "System.h"

#include "AssertObject.h"
#include "detail/EngineCore.h"

namespace
{
    bool s_initialFrame{true}; // FIXME?
}

namespace ZG
{
    using namespace detail;

    bool System::Update()
    {
        if (not s_initialFrame) EngineCore.EndFrame();
        s_initialFrame = false;

        MSG msg;
        if (PeekMessage(&msg, nullptr, 0, 0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            return false;
        }

        EngineCore.BeginFrame();
        return true;
    }
}
