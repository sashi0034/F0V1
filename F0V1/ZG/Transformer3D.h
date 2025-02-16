﻿#pragma once
#include "Mat4x4.h"
#include "Uncopyable.h"

namespace ZG
{
    class Transformer3D : Uncopyable
    {
    public:
        Transformer3D() = default;

        Transformer3D(const Mat4x4& localWorldMat);

        ~Transformer3D();

    private:
        bool m_active{};
    };
}
