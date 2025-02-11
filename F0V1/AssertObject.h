#pragma once

#include <source_location>
#include <string>

#include "Windows.h"

namespace ZG
{
    struct AssertObject
    {
        std::string_view errorMessage;
#ifdef _DEBUG
        std::source_location location{};

        AssertObject(
            std::string_view errorMessage,
            const std::source_location& location = std::source_location::current());
#else
        AssertObject(std::string_view errorMessage);
#endif

        [[noreturn]] void throwError() const;
    };

    struct Assert_HRESULT : AssertObject
    {
        Assert_HRESULT operator |(HRESULT result) const;
    };
}
