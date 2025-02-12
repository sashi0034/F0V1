#pragma once
#include <d3dcommon.h>
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

        void PushPS(ID3DBlob* psBlob) const;
        void PopPS() const;

        void PushVS(ID3DBlob* vsBlob) const;
        void PopVS() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> p_impl;
    };

    inline const auto EngineCore = EngineCore_impl{};
}
