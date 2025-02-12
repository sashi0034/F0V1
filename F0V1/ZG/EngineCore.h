#pragma once
#include <memory>

namespace ZG
{
    class EngineCore
    {
    public:
        EngineCore();

        void Init();

        void Update();

        void Destroy();

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };
}
