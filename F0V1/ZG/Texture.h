#pragma once

namespace ZG
{
    struct TextureParams
    {
        std::wstring filename;
    };

    class Texture
    {
    public:
        Texture(const TextureParams& params);

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
