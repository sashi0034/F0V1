#pragma once
#include <DirectXMath.h>

namespace ZG
{
    struct Buffer3DParams
    {
        std::vector<DirectX::XMFLOAT3> vertexes;
        std::vector<uint16_t> indices;
    };

    struct Buffer3D_impl;

    class Buffer3D
    {
    public:
        Buffer3D(const Buffer3DParams& params);

        void Draw() const;

    private:
        std::shared_ptr<Buffer3D_impl> p_impl;
    };
}
