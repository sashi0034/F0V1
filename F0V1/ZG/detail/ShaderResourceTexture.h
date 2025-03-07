#pragma once

namespace ZG
{
    class Image;
}

namespace ZG::detail
{
    class ShaderResourceTexture
    {
    public:
        ShaderResourceTexture() = default;

        ShaderResourceTexture(const std::wstring& filename);

        ShaderResourceTexture(const Image& image);

        bool isEmpty() const;

        ID3D12Resource* getResource() const;

        DXGI_FORMAT getFormat() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
