#include "pch.h"
#include "EngineEntryPoint.h"

#include "Windows.h"
#include "ZG/Buffer3D.h"

#include "ZG/Logger.h"
#include "ZG/Utils.h"
#include "ZG/detail/EngineCore.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
}

void Main();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Logger.HR().Writeln(L"application start");

    EngineCore.Init();

    Logger.HR().Writeln(L"message loop start");

    // -----------------------------------------------

    try
    {
        Main();
    }
    catch (const std::exception& e)
    {
        Logger.HR().Writeln(L"exception occurred: " + ToUtf16(e.what()));
    }

    // -----------------------------------------------

    Logger.HR().Writeln(L"message loop end");

    EngineCore.Destroy();

    Logger.HR().Writeln(L"application end");

    return 0;
}
