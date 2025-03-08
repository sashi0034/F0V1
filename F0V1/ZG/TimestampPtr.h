#pragma once

// namespace ZG
// {
//     constexpr uint64_t InvalidTimestamp = static_cast<int64_t>(-1);
//
//     class TimestampPtr
//     {
//     public:
//         explicit TimestampPtr(uint64_t timestamp = InvalidTimestamp)
//             : m_timestamp(std::make_shared<uint64_t>(timestamp))
//         {
//         }
//
//         void set(uint64_t timestamp) { *m_timestamp = timestamp; }
//
//         uint64_t get() const { return *m_timestamp; }
//
//     private:
//         std::shared_ptr<uint64_t> m_timestamp;
//     };
//
//     static inline const TimestampPtr InvalidTimestampPtr{InvalidTimestamp};
// }
