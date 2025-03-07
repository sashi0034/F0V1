#include "pch.h"
#include "EnginePresetAsset.h"

#include "ShaderResourceTexture.h"
#include "ZG/Image.h"
#include "ZG/Value2D.h"

using namespace ZG;

using namespace ZG::detail;

namespace
{
    struct Impl
    {
        ShaderResourceTexture m_whiteTexture;

        Impl()
        {
            const Image whiteImage{Size{16, 16}, ColorU8{255}};
            m_whiteTexture = ShaderResourceTexture(whiteImage);
        }
    };

    std::shared_ptr<Impl> s_enginePresetAsset{};
}

namespace ZG::detail
{
    void EnginePresetAsset_impl::Init() const
    {
        s_enginePresetAsset = std::make_shared<Impl>();
    }

    void EnginePresetAsset_impl::Destroy() const
    {
        s_enginePresetAsset.reset();
    }

    ShaderResourceTexture EnginePresetAsset_impl::GetWhiteTexture() const
    {
        return s_enginePresetAsset->m_whiteTexture;
    }
}
