#pragma once

// 1. Project Headers
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <memory>
#include <thread>

namespace N503::Renderer2D
{

    class Engine final
    {
    public:
        static auto GetInstance() noexcept -> Engine&;

    public:
        Engine();

    public:
        auto Start() -> void;

        auto Stop() -> void;

        auto Wait() -> void;

        auto Run(const std::stop_token stopToken) -> void;

    public:
        auto GetMessageQueue() const -> Message::Queue&
        {
            return *m_MessageQueue;
        }

        auto GetDiagnosticsSink() const -> const Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

        auto GetDiagnosticsSink() -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Renderer2D.Event.EngineStarted") };

        Diagnostics::Sink m_DiagnosticsSink;

        std::unique_ptr<Message::Queue> m_MessageQueue;

        std::jthread m_RendererThread;
    };

} // namespace N503::Renderer2D
