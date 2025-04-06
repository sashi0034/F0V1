#pragma once
#include "Script/Script_asapi.h"

namespace ZG
{
    namespace System
    {
        ASAPI_NAMESPACE("System");

        bool Update();
        ASAPI_GLOBAL_FUNCTION("bool Update()", Update);

        double DeltaTime();
        ASAPI_GLOBAL_FUNCTION("double DeltaTime()", DeltaTime);

        uint64_t FrameCount();
        ASAPI_GLOBAL_FUNCTION("uint64 FrameCount()", FrameCount);

        void ModalError(const std::wstring& message);

        void ModalError(const std::string& message);
        ASAPI_GLOBAL_FUNCTION_BY(
            "void ModalError(const string& in message)",
            <const std::string&>(System::ModalError));
    }
}
