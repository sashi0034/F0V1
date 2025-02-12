#pragma once
#include <memory>

#include "IResource.h"

namespace ZG
{
    class EngineCore_impl
    {
    public:
        EngineCore_impl();

        void Init() const;

        void Update() const;

        void Destroy() const;

        std::shared_ptr<IResourceState> CreateResource(const IResourceProps& props) const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    inline const auto EngineCore = EngineCore_impl{};
}
