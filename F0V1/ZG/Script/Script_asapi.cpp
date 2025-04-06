#include "pch.h"

#include "Script_asapi.h"

namespace
{
    std::string preprocessDeclaration(std::string declarations, std::map<std::string, std::string> macros)
    {
        for (const auto& [key, value] : macros)
        {
            size_t pos = 0;
            while ((pos = declarations.find(key, pos)) != std::string::npos)
            {
                declarations.replace(pos, key.length(), value);
                pos += value.length(); // 次の位置へ進む
            }
        }

        return declarations;
    }
}

namespace ZG::asapi_detail
{
    std::function<std::string(std::string)> MacroPreprocessor(const std::map<std::string, std::string>& macros)
    {
        return [macros](const std::string& declarations) -> std::string
        {
            return preprocessDeclaration(declarations, macros);
        };
    }
}
