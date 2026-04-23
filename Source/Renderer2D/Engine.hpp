#pragma once

// 1. Project Headers
#include "Command/Dispatcher.hpp"
#include "Command/Queue.hpp"
#include "Device/Context.hpp"
#include "Processor.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <memory>
#include <stop_token>
#include <thread>

namespace N503::Renderer2D
{

    class Engine final
    {
    public:
        static auto Instance() -> Engine&;

    public:
        ~Engine();

        Engine(const Engine&) = delete;

        auto operator=(const Engine&) -> Engine& = delete;

        Engine(Engine&&) = delete;

        auto operator=(Engine&&) -> Engine& = delete;

    public:
        auto Start() -> void;

        auto Stop() -> void;

        auto Wait() -> void;

        auto GetCommandQueue() -> Command::Queue&
        {
            return *m_CommandQueue;
        }

        auto GetDeviceContext() -> Device::Context&
        {
            return *m_DeviceContext;
        }

        auto GetResourceContainer() -> Resource::Container&
        {
            return *m_ResourceContainer;
        }

        auto GetBatchProcessor() -> Renderer2D::Processor&
        {
            return *m_BatchProcessor;
        }

        auto GetDiagnosticsSink() -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

    private:
        Engine();

        auto Run(std::stop_token stopToken) -> void;

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Renderer2D.Event.EngineStarted") };

        std::unique_ptr<Command::Queue> m_CommandQueue;

        std::unique_ptr<Renderer2D::Processor> m_BatchProcessor;

        std::unique_ptr<Resource::Container> m_ResourceContainer;

        std::unique_ptr<Device::Context> m_DeviceContext;

        Diagnostics::Sink m_DiagnosticsSink;

        std::jthread m_RenderThread;
    };

} // namespace N503::Renderer2D
