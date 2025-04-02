#pragma once
#include "ZG/Image.h"
#include "ZG/Variant.h"

namespace ZG::detail
{
    class ShaderResourceTexture
    {
    public:
        ShaderResourceTexture() = default;

        ShaderResourceTexture(const Variant<std::wstring, Image, ID3D12Resource*>& source);

        bool isEmpty() const;

        ID3D12Resource* getResource() const;

        DXGI_FORMAT getFormat() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
