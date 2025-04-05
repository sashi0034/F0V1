#pragma once
#include "ZG/Image.h"
#include "ZG/Variant.h"

namespace ZG::detail
{
    class ShaderResourceTexture
    {
    public:
        ShaderResourceTexture() = default;

        ShaderResourceTexture(const Variant<std::string, Image, ID3D12Resource*>& source);

        bool isEmpty() const;

        Size size() const;

        ID3D12Resource* getResource() const;

        DXGI_FORMAT getFormat() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
