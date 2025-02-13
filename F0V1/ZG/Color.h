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

    struct ColorU8
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        uint8_t* GetPointer() { return &r; }
    };
}
