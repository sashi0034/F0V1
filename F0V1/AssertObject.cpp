#include "AssertObject.h"

#include <stdexcept>

namespace
{
    std::wstring string_to_wstring(const std::string& str)
    {
        // Get the required buffer size for the wide string
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

        // Create a buffer to hold the wide string
        std::wstring wstr(size_needed, 0);

        // Perform the conversion
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);

        // Remove the null terminator added by MultiByteToWideChar
        wstr.resize(size_needed - 1);

        return wstr;
    }
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
        const auto message = string_to_wstring(std::string(errorMessage));
        const auto filename = string_to_wstring(location.file_name());
        const std::wstring output =
            message + L"\n" + filename + L"(" + std::to_wstring(location.line()) + L")";

        // TODO: エラーっぽくする
        MessageBox(nullptr, output.c_str(), L"Error", MB_OK);
#endif

        if (not errorMessage.empty()) throw std::runtime_error(std::string(errorMessage));
        else throw std::runtime_error("An error occurred");
    }

    Assert_HRESULT Assert_HRESULT::operator|(HRESULT result) const
    {
        if (FAILED(result))
        {
            throwError();
        }

        return *this;
    }
}
