#pragma once

// 1. Project Headers
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Pool.hpp>
#include <N503/Memory/StorageAllocator.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <list>
#include <mutex>
#include <queue>
#include <utility>
#include <variant>

namespace N503::Renderer2D::Message
{

    class Queue final
    {
        static const std::size_t MaxMessageQueued = 4096;

        static const std::size_t BufferCount = 3;

    public:
        using Element = Envelope;

        struct Slot
        {
            alignas(void*) std::byte data[sizeof(Element) + 32];
        };

        using Storage = ::N503::Memory::Storage::Pool<Slot>;

        using Allocator = ::N503::Memory::StorageAllocator<Element, Storage>;

        using Strategy = std::list<Element, Allocator>;

        using Container = std::queue<Element, Strategy>;

    public:
        template <typename TPacket> class MessageBorrower
        {
        public:
            MessageBorrower(std::unique_lock<std::mutex>&& lock, Container& queue) : m_Lock(std::move(lock))
            {
                if (!queue.empty() && std::holds_alternative<TPacket>(queue.back().Packet))
                {
                    m_Target = &std::get_if<TPacket>(queue.back().Packet);
                }
            }

            MessageBorrower(const MessageBorrower&) = delete;

            auto operator=(const MessageBorrower&) -> MessageBorrower& = delete;

            auto operator->() -> TPacket*
            {
                return m_Target;
            }

            explicit operator bool() const
            {
                return m_Target != nullptr;
            }

        private:
            std::unique_lock<std::mutex> m_Lock;

            TPacket* m_Target = nullptr;
        };

    public:
        Queue() = default;

        Queue(const Queue&) = delete;

        auto operator=(const Queue&) -> Queue& = delete;

        Queue(Queue&&) = delete;

        auto operator=(Queue&&) -> Queue& = delete;

    public:
        auto Enqueue(Packet&&) -> void;

        auto EnqueueSync(Packet&&) -> void;

        auto Enqueue(std::vector<Packet>&&) -> void;

        auto EnqueueSync(std::vector<Packet>&&) -> void;

        [[nodiscard]]
        auto PopAll() -> Container;

        [[nodiscard]]
        auto GetWakeupEventHandle() const -> HANDLE;

    public:
        template <typename TDataType> auto TryBorrowBack() -> MessageBorrower<TDataType>
        {
            // ロックを奪取する
            std::unique_lock<std::mutex> lock{ m_Mutex };

            // ロックした状態で Container を特定し、lock ごと Borrower に渡す
            return MessageBorrower<TDataType>{ std::move(lock), m_Buffer[m_BufferIndex].Container };
        }

    private:
        /// @brief
        struct Buffer
        {
            typename Queue::Storage Storage{ MaxMessageQueued }; ///< Allocator より先に宣言する必要がある事に注意

            typename Queue::Allocator Allocator{ &Storage }; ///< Container より先に宣言する必要がある事に注意

            typename Queue::Container Container{ Allocator };
        };

        std::array<Buffer, BufferCount> m_Buffer{};

        std::uint32_t m_BufferIndex{ 0 };

        wil::unique_event_nothrow m_WakeupEvent{ ::CreateEventW(nullptr, FALSE, FALSE, nullptr) };

        std::mutex m_Mutex;

        std::atomic_flag m_SpinLock = ATOMIC_FLAG_INIT;
    };

} // namespace N503::Renderer2D::Message
