#include "Pch.hpp"
#include "Queue.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <format>
#include <semaphore>
#include <utility>

namespace N503::Renderer2D::Message
{

    auto Queue::Enqueue(Packet&& packet) -> void
    {
        bool isBusy = false;

#ifdef _DEBUG
        {
            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued;

            if (currentSize >= capacity * 0.8) // 80%を超えたら警告
            {
                Engine::GetInstance().GetDiagnosticsSink().AddEntry(Diagnostics::Entry{
                    .Severity = Diagnostics::Severity::Warning,
                    .Expected = std::format("EventQueue is congesting: CurrentSize={}, Capacity={}\n", currentSize, capacity).data(),
                    .Position = 0,
                });
            }
        }
#endif
        {
            std::scoped_lock lock(m_Mutex);

            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued;

            if (currentSize >= capacity * 0.95)
            {
                isBusy = true;
            }
            else
            {
                m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(packet) });
            }
        }

        if (isBusy)
        {
            EnqueueSync(std::move(packet));
        }
        else
        {
            m_WakeupEvent.SetEvent();
        }
    }

    auto Queue::EnqueueSync(Packet&& packet) -> void
    {
        std::binary_semaphore signal{ 0 };

#ifdef _DEBUG
        {
            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued;

            if (currentSize >= capacity * 0.8) // 80%を超えたら警告
            {
                Engine::GetInstance().GetDiagnosticsSink().AddEntry(Diagnostics::Entry{
                    .Severity = Diagnostics::Severity::Warning,
                    .Expected = std::format("EventQueue is congesting: CurrentSize={}, Capacity={}\n", currentSize, capacity).data(),
                    .Position = 0 });
            }
        }
#endif
        {
            std::scoped_lock lock(m_Mutex);

            m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(packet), &signal });
        }

        m_WakeupEvent.SetEvent();

        signal.acquire();
    }

    auto Queue::Enqueue(std::vector<Packet>&& packets) -> void
    {
        while (!packets.empty())
        {
            std::scoped_lock lock(m_Mutex);

            const std::size_t currentSize  = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t canPushCount = (currentSize < MaxMessageQueued) ? (MaxMessageQueued - currentSize) : 0;
            const std::size_t processCount = std::min(packets.size(), canPushCount);

            if (processCount > 0)
            {
                auto first = packets.begin();
                auto last  = packets.begin() + processCount;

                std::vector<Packet> bundle(std::make_move_iterator(first), std::make_move_iterator(last));

                packets.erase(first, last);

                m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(bundle), nullptr });
                m_WakeupEvent.SetEvent();
            }

            if (!packets.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }

    auto Queue::EnqueueSync(std::vector<Packet>&& packets) -> void
    {
        std::binary_semaphore signal{ 0 };

        while (!packets.empty())
        {
            std::scoped_lock lock(m_Mutex);

            const std::size_t currentSize  = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t canPushCount = (currentSize < MaxMessageQueued) ? (MaxMessageQueued - currentSize) : 0;
            const std::size_t processCount = std::min(packets.size(), canPushCount);

            if (processCount > 0)
            {
                auto first = packets.begin();
                auto last  = packets.begin() + processCount;

                std::vector<Packet> bundle(std::make_move_iterator(first), std::make_move_iterator(last));

                packets.erase(first, last);

                m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(bundle), &signal });
                m_WakeupEvent.SetEvent();
            }

            if (!packets.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        signal.acquire();
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::PopAll() -> Container
    {
        std::scoped_lock lock(m_Mutex);

        // UIスレッドで溜めたデータを「Allocatorごと」奪い去る
        auto container = std::move(m_Buffer[m_BufferIndex].Container);

        // 空になった場所に、正しい Allocator（Storageへのポインタ）を再装填
        m_Buffer[m_BufferIndex].Container = Container{ m_Buffer[m_BufferIndex].Allocator };

        // 次の Push 用にインデックスを切り替える
        m_BufferIndex = (m_BufferIndex + 1) % m_Buffer.size();

        return container;
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::GetWakeupEventHandle() const -> HANDLE
    {
        return m_WakeupEvent.get();
    }

} // namespace N503::Renderer2D::Message
