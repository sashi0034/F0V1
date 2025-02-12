﻿#include "pch.h"
#include "AssertObject.h"

#include <stdexcept>

#include "Utils.h"

namespace
{
}

namespace ZG
{
#if _DEBUG
    AssertObject::AssertObject(std::string_view errorMessage, const std::source_location& location) :
        errorMessage(errorMessage),
        location(location)
    {
    }
#else
    AssertObject::AssertObject(std::string_view errorMessage) :
        errorMessage(errorMessage)
    {
    }
#endif

    void AssertObject::throwError() const
    {
#if _DEBUG
        const auto message = ToUtf16(std::string(errorMessage));
        const auto filename = ToUtf16(location.file_name());
        const std::wstring output =
            message + L"\n" + filename + L":" + std::to_wstring(location.line());

        MessageBox(nullptr, output.c_str(), L"assertion failed", MB_OK | MB_ICONERROR);
#endif

        if (not errorMessage.empty()) throw std::runtime_error(std::string(errorMessage));
        else throw std::runtime_error("An error occurred");
    }

    AssertWin32 AssertWin32::operator|(HRESULT result) const
    {
        if (FAILED(result))
        {
            throwError();
        }

        return *this;
    }
}
