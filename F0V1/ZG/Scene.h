#pragma once
#include "Value2D.h"

namespace ZG
{
    namespace Scene
    {
        ASAPI_NAMESPACE("Scene");

        [[nodiscard]] ZG::Size Size();
        ASAPI_GLOBAL_BIND(function("Point Size()", Size));

        [[nodiscard]] Point Center();
        ASAPI_GLOBAL_BIND(function("Point Center()", Center));
    }
}
