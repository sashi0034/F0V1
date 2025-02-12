#include "pch.h"
#include "Logger.h"

#include "AssertObject.h"

namespace
{
    bool s_initializedConsole = false;

    bool s_flushRequest{};
}

namespace ZG
{
    const Logger_impl& Logger_impl::hr() const
    {
        writeln(L"--------------------------------------------------");
        return *this;
    }

    void Logger_impl::writeln(const std::wstring& message) const
    {
        if (not s_initializedConsole)
        {
            s_initializedConsole = true;

            if (AllocConsole())
            {
                FILE* fp = nullptr;
                freopen_s(&fp, "CONOUT$", "w", stdout);
            }
        }

        if (s_flushRequest)
        {
            s_flushRequest = false;
            std::wcout << std::endl;
        }

        // OutputDebugString(message.c_str());
        std::wcout << message;

        s_flushRequest = true;
    }

    const Logger_impl& Logger_impl::operator<<(const std::wstring& message) const
    {
        writeln(message);
        return *this;
    }
}
