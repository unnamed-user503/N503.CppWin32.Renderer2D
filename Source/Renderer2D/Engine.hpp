#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <memory>
#include <thread>

namespace N503::Renderer2D::Message
{
    class Queue;
}

namespace N503::Renderer2D::System
{
    class Registry;
}

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

        auto GetSystemRegistry() const -> System::Registry&
        {
            return *m_SystemRegistry;
        }

        auto GetDiagnosticsReporter() -> Diagnostics::Reporter&
        {
            return *m_DiagnosticsReporter;
        }

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Renderer2D.Event.EngineStarted") };

        std::unique_ptr<System::Registry> m_SystemRegistry;

        std::unique_ptr<Message::Queue> m_MessageQueue;

        std::jthread m_RendererThread;

        std::unique_ptr<Diagnostics::Reporter> m_DiagnosticsReporter;
    };

} // namespace N503::Renderer2D
