#pragma once

namespace ZG
{
    struct ColorF32
    {
        float r;
        float g;
        float b;
        float a;

        float* GetPointer() { return &r; }
    };
}
