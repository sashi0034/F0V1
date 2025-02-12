#pragma once
#include <string>

namespace ZG
{
    class Logger_impl
    {
    public:
        /// @brief Write a horizontal rule
        const Logger_impl& HR() const;

        void Writeln(const std::wstring& message) const;

        const Logger_impl& operator <<(const std::wstring& message) const;
    };

    static inline constexpr auto Logger = Logger_impl{};
}
