#pragma once
#include "Image.h"

namespace ZG
{
    class Texture
    {
    public:
        Texture(std::wstring filename);

        Texture(const Image& image);

        void Draw() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
        std::wstring m_filename;
    };
}
