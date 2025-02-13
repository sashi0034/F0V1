#pragma once
#include <string>

namespace ZG
{
    std::wstring ToUtf16(const std::string& str);

    /// @brief アライメントに揃えたサイズを取得する
    constexpr size_t AlignedSize(size_t size, size_t alignment)
    {
        return size + alignment - (size % alignment);
    }
}
