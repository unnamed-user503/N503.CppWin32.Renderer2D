#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Command/Dispatcher.hpp"
#include "Command/Packets/DestroyResourceCommand.hpp"
#include "Command/Queue.hpp"
#include "Device/Context.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <format>
#include <memory>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <utility>

namespace N503::Renderer2D
{

    namespace
    {
        auto TryGetTargetWindowHandle() -> HWND
        {
            wil::unique_event_nothrow createWindowCompletion;

            if (!createWindowCompletion.try_open(L"Local\\N503.CppWin32.Event.Core.CreateWindowCompletion", SYNCHRONIZE | EVENT_MODIFY_STATE))
            {
                return {};
            }

            if (::WaitForSingleObject(createWindowCompletion.get(), 0) != WAIT_OBJECT_0)
            {
                return {};
            }

            struct Context
            {
                HWND Result{};
                DWORD TargetPID{};
            };

            Context context{ nullptr, ::GetCurrentProcessId() };

            auto callback = [](HWND handle, LPARAM lParam) -> BOOL
            {
                auto pContext = reinterpret_cast<Context*>(lParam);

                DWORD pid{};
                ::GetWindowThreadProcessId(handle, &pid);

                if (pid == pContext->TargetPID)
                {
                    wchar_t className[256]{};
                    ::GetClassNameW(handle, className, 256);

                    if (std::wcscmp(className, L"N503.CppWin32.Core.Window") == 0)
                    {
                        pContext->Result = handle;
                        return FALSE;
                    }
                }

                return TRUE;
            };

            ::EnumWindows(callback, reinterpret_cast<LPARAM>(&context));

            if (!context.Result)
            {
                return {};
            }
            else
            {
                // 成功したので旗を下ろす
                createWindowCompletion.ResetEvent();
            }

            return context.Result;
        }
    } // namespace

    auto Engine::Instance() -> Engine&
    {
        static Engine renderer2DEngine;
        return renderer2DEngine;
    }

    Engine::Engine()
    {
        m_CommandQueue = std::make_unique<Command::Queue>();
    }

    Engine::~Engine()
    {
        Stop();
        Wait();
    }

    /// @brief
    /// @return
    auto Engine::Start() -> void
    {
        if (m_IsRunning.load(std::memory_order_acquire))
        {
            return;
        }

        // スレッド開始同期用シグナル
        std::binary_semaphore signal{ 0 };

        m_RenderThread = std::jthread(
            [this, &signal](std::stop_token stopToken)
            {
                // スレッドに名前を付ける
                ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Renderer2D");

                // スレッドが開始されたのでロックを解放する
                signal.release();

                // エンジンスレッドが起動したので旗を立てる
                m_StartedEvent.SetEvent();

                // COMの初期化
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);

                // UIスレッドの開始
                this->Run(std::move(stopToken));

                // エンジンスレッドが停止したので旗を下す
                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        // スレッドが開始されるのを待つ
        signal.acquire();

        // エンジンスレッドが停止したので旗を立てる
        m_IsRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_RenderThread.native_handle()), WM_QUIT, 0, 0))
        {
            const Diagnostics::Entry entry{
                .Severity = Diagnostics::Severity::Error,
                .Expected = std::format("PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_RenderThread.native_handle()).data(),
                .Position = 0
            };

            m_DiagnosticsSink.AddEntry(entry);
        }
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        m_DeviceContext     = std::make_unique<Device::Context>();
        m_ResourceContainer = std::make_unique<Resource::Container>();
        m_BatchProcessor    = std::make_unique<Renderer2D::Processor>();

        auto CleanupResources = wil::scope_exit(
            [&]
            {
                m_DeviceContext.reset();
                m_ResourceContainer.reset();
                m_BatchProcessor.reset();

                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        auto OSMessageDispatch = []() -> bool
        {
            MSG message{};
            while (::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    return false;
                }
                ::TranslateMessage(&message);
                ::DispatchMessageW(&message);
            }
            return true;
        };

        auto wakeupHandles = { m_CommandQueue->GetWakeupEventHandle() };

        Command::Dispatcher commandDispatcher;
        Diagnostics::Reporter diagnosticsReporter;
        diagnosticsReporter.AddSink(std::make_shared<Diagnostics::ConsoleSink>());

        bool isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            if (auto hwnd = TryGetTargetWindowHandle())
            {
                m_DeviceContext->SetRenderTarget(hwnd);
            }

            const auto count   = static_cast<DWORD>(wakeupHandles.size());
            const auto handles = wakeupHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                commandDispatcher.Dispatch(*m_CommandQueue);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!OSMessageDispatch())
                {
                    return;
                }
            }

            if (m_DeviceContext->BeginDraw())
            {
                isAnyActive = m_BatchProcessor->Process(*m_DeviceContext);
                m_DeviceContext->EndDraw();
                m_DeviceContext->Present();
            }

#ifdef _DEBUG
            diagnosticsReporter.Submit(m_DiagnosticsSink);
#endif
            std::this_thread::yield();
        }

        diagnosticsReporter.Stop();
        diagnosticsReporter.Wait();
    }

    auto Engine::Wait() -> void
    {
        if (m_RenderThread.joinable())
        {
            m_RenderThread.join();
        }
    }

} // namespace N503::Renderer2D
