#pragma once

namespace ZG
{
    namespace System
    {
        bool Update();

        double DeltaTime();

        void ModalError(const std::wstring& message);
    }
}
