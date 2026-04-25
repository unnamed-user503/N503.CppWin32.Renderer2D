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
            const std::size_t capacity    = MaxMessageQueued; // プールのサイズ

            if (currentSize >= capacity * 0.8) // 80%を超えたら警告
            {
                Engine::GetInstance().GetDiagnosticsSink().AddEntry(
                    Diagnostics::Entry{
                        .Severity = Diagnostics::Severity::Warning,
                        .Expected = std::format("EventQueue is congesting: CurrentSize={}, Capacity={}\n", currentSize, capacity).data(),
                        .Position = 0
                    }
                );
            }
        }
#endif
        {
            const std::lock_guard lock{ m_Mutex };

            const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t capacity    = MaxMessageQueued; // プールのサイズ

            if (currentSize >= capacity * 0.95)
            {
                isBusy = true;
            }
            else
            {
                m_Buffer[m_BufferIndex].Container.push(Envelope{ .Packet = std::move(packet), .Signal = nullptr });
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
            const std::size_t capacity    = MaxMessageQueued; // プールのサイズ

            if (currentSize >= capacity * 0.8) // 80%を超えたら警告
            {
                Engine::GetInstance().GetDiagnosticsSink().AddEntry(
                    Diagnostics::Entry{
                        .Severity = Diagnostics::Severity::Warning,
                        .Expected = std::format("EventQueue is congesting: CurrentSize={}, Capacity={}\n", currentSize, capacity).data(),
                        .Position = 0
                    }
                );
            }
        }
#endif
        {
            const std::lock_guard lock{ m_Mutex };

            m_Buffer[m_BufferIndex].Container.push(Envelope{ .Packet = std::move(packet), .Signal = &signal });
        }

        m_WakeupEvent.SetEvent();

        signal.acquire();
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::PopAll() -> Container
    {
        const std::lock_guard lock{ m_Mutex };

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
