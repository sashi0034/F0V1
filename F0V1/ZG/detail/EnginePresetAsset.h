#pragma once
#include "ShaderResourceTexture.h"

namespace ZG::detail
{
    class EnginePresetAsset_impl
    {
    public:
        void Init() const;

        void Destroy() const;

        ShaderResourceTexture GetWhiteTexture() const;
    };

    inline constexpr auto EnginePresetAsset = EnginePresetAsset_impl{};
}
