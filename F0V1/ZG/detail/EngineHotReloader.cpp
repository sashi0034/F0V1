#include "pch.h"
#include "EngineHotReloader.h"

#include "ZG/Logger.h"

using namespace ZG;
using namespace ZG::detail;

namespace
{
    struct TrackingElement
    {
        std::weak_ptr<IEngineHotReloadable> target;
        Array<std::shared_ptr<ITimestamp>> dependencies;
    };

    bool checkHotReloadElement(IEngineHotReloadable& target, const Array<std::shared_ptr<ITimestamp>>& dependencies)
    {
        const auto targetTimestamp = target.timestamp();

        for (const auto& dependency : dependencies)
        {
            if (targetTimestamp < dependency->timestamp())
            {
                target.HotReload();
                return true;
            }
        }

        return false;
    }

    struct Impl
    {
        Array<TrackingElement> m_elements{};

        void Update()
        {
            for (int i = 0; i < m_elements.size(); ++i)
            {
                bool reloaded = checkHotReload();
                if (not reloaded) break;

                if (i == m_elements.size() - 1)
                {
                    LogWarning.Writeln(L"the timestamps of some elements have not been updated in hot reload");
                    break;
                }
            }
        }

        bool checkHotReload()
        {
            bool hotReloaded{};
            for (int i = m_elements.size() - 1; i >= 0; --i)
            {
                auto& element = m_elements[i];
                if (auto target = element.target.lock())
                {
                    hotReloaded |= checkHotReloadElement(*target, element.dependencies);
                }
                else
                {
                    m_elements.erase(m_elements.begin() + i);
                }
            }

            return hotReloaded;
        }
    };

    Impl s_hotReloader{};
}

namespace ZG
{
    void EngineHotReloader_impl::Update() const
    {
        s_hotReloader.Update();
    }

    void EngineHotReloader_impl::Destroy() const
    {
        s_hotReloader.m_elements.clear();
    }

    void EngineHotReloader_impl::TrackAsset(
        std::weak_ptr<IEngineHotReloadable> target,
        Array<std::shared_ptr<ITimestamp>> dependencies
    ) const
    {
#ifdef  _DEBUG
        s_hotReloader.m_elements.emplace_back(target, std::move(dependencies));
#endif
    }
}
