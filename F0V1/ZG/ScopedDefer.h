#pragma once

namespace ZG
{
    class ScopedDefer
    {
    public:
        ScopedDefer() = default;

        explicit ScopedDefer(const std::function<void()>& func);

        ScopedDefer(const ScopedDefer&) = delete;

        ScopedDefer& operator=(const ScopedDefer&) = delete;

        ScopedDefer(ScopedDefer&& other) noexcept;

        ScopedDefer& operator=(ScopedDefer&& other) noexcept;

        ~ScopedDefer();

    private:
        std::function<void()> m_func;
        bool m_active;

    protected:
        void dispose();
    };

    class DisposableScopedDefer : public ScopedDefer
    {
    public:
        /// @brief 遅延実行する予定であった処理を強制的に実行してから解除する
        void Dispose();
    };
}
