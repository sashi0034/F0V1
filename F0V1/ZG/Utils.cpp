#include "pch.h"
#include "Utils.h"

#include "AssertObject.h"

namespace ZG
{
    std::wstring ToUtf16(const std::string& str)
    {
        // Get the required buffer size for the wide string
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

        // Create a buffer to hold the wide string
        std::wstring wstr(size_needed, 0);

        // Perform the conversion
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr.data(), size_needed);

        // Remove the null terminator added by MultiByteToWideChar
        wstr.resize(size_needed - 1);

        return wstr;
    }

    std::wstring StringifyBlob(ID3DBlob* blob)
    {
        return ToUtf16(std::string{static_cast<char*>(blob->GetBufferPointer()), blob->GetBufferSize()});
    }
}
