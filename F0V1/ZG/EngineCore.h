#pragma once
#include <memory>

namespace ZG
{
    class ResourceFactory;

    class EngineCore_impl
    {
    public:
        EngineCore_impl();

        void Init() const;

        void Update() const;

        void Destroy() const;

        ResourceFactory GetResourceFactory() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    inline const auto EngineCore = EngineCore_impl{};
}
