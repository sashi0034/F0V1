#pragma once

namespace ZG::detail
{
    class EngineTimer_impl
    {
    public:
        void Reset() const;

        void Tick() const;

        double GetDeltaTime() const;
    };

    inline constexpr auto EngineTimer = EngineTimer_impl{};
}
