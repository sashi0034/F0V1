#pragma once
#include "ZG/ITimestamp.h"

namespace ZG::detail
{
    class IEngineInternalAsset : public ITimestamp
    {
    public:
        virtual void HotReload() = 0;
    };
}
