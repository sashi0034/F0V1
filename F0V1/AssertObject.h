#pragma once

#include "Windows.h"

namespace ZG
{
    struct Assert_HRESULT
    {
        void operator <<(HRESULT result) const;
    };
}
