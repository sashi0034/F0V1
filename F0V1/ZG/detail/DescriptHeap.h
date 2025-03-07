#pragma once
#include "DescriptorTable.h"
#include "ShaderResourceTexture.h"

namespace ZG::detail
{
    struct CbvSrvUavSource
    {
        Array<ShaderResourceTexture> textures;
    };

    struct DescriptorHeapParams
    {
        DescriptorTable table;
    };

    class DescriptHeap
    {
    public:

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
