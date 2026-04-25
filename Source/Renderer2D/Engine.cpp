#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Device/CommandList.hpp"
#include "Device/Context.hpp"
#include "Device/RenderTarget.hpp"
#include "Message/Context.hpp"
#include "Message/Dispatcher.hpp"
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <format>
#include <memory>
#include <semaphore>
#include <thread>
#include <utility>

namespace N503::Renderer2D
{

    auto Engine::GetInstance() noexcept -> Engine&
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine()
    {
        // リソース コンテナを初期化する
        m_ResourceContainer = std::make_unique<Resource::Container>();
        // メッセージ キューを初期化する
        m_MessageQueue = std::make_unique<Message::Queue>();
    }

    auto Engine::Start() -> void
    {
        // すでにエンジンが開始されている場合は、何もしない
        if (m_IsRunning.load(std::memory_order_acquire))
        {
            return;
        }

        std::binary_semaphore signal{ 0 };

        // レンダラー スレッドを作成する
        m_RendererThread = std::jthread(
            [this, &signal](const std::stop_token stopToken)
            {
                // スレッド名を設定する
                ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Renderer2D");

                // レンダラー スレッドの初期化が完了したことを通知する
                signal.release();

                // レンダラー スレッドの初期化が完了したことを示すイベントをシグナル状態にする
                m_StartedEvent.SetEvent();

                // COM を初期化する
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);

                // PeekMessageWを呼び出して、レンダラースレッドのメッセージキューを作成する
                MSG msg{};
                ::PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

                // レンダラースレッドを開始する
                Run(std::move(stopToken));

                // レンダラー スレッドが停止したことを示すイベントをリセット状態にする
                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        // スレッドが起動して初期化が完了するまで待機する
        signal.acquire();

        // エンジンが開始されたことを示すイベントをシグナル状態にする
        m_IsRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_RendererThread.native_handle()), WM_QUIT, 0, 0))
        {
            const Diagnostics::Entry entry{
                .Severity = Diagnostics::Severity::Error,
                .Expected = std::format("PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_RendererThread.native_handle()).data(),
                .Position = 0
            };

            m_DiagnosticsSink.AddEntry(entry);
        }
    }

    auto Engine::Wait() -> void
    {
        if (m_RendererThread.joinable())
        {
            m_RendererThread.join();
        }
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        Diagnostics::Reporter diagnosticsReporter;
        diagnosticsReporter.AddSink(std::make_shared<Diagnostics::ConsoleSink>());

        std::unique_ptr<Device::Context> deviceContext     = std::make_unique<Device::Context>();
        std::unique_ptr<Device::RenderTarget> renderTarget = nullptr;
        std::unique_ptr<Device::CommandList> commandList   = std::make_unique<Device::CommandList>();

        // メッセージ ディスパッチャーを初期化する
        Message::Dispatcher messageDispatcher;

        // レンダラースレッドの終了時にリソースをクリーンアップするためのスコープ ガード
        auto cleanupResources = wil::scope_exit(
            [&]
            {
                renderTarget.reset();
                deviceContext.reset();
                commandList.reset();
            }
        );

        // OS メッセージをディスパッチするラムダ式
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

        // レンダラー スレッドのメイン ループ
        auto waitHandles = { m_MessageQueue->GetWakeupEventHandle() };
        auto isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            HWND hwnd{};

            if (renderTarget == nullptr && (hwnd = deviceContext->GetRenderTargetWindow()))
            {
                renderTarget = std::make_unique<Device::RenderTarget>(deviceContext.get(), hwnd);
                deviceContext->SetTarget(*renderTarget);
            }

            const auto count   = static_cast<DWORD>(waitHandles.size());
            const auto handles = waitHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                Message::Context context{
                    .MessageQueue      = *m_MessageQueue,
                    .DeviceContext     = *deviceContext,
                    .CommandList       = *commandList,
                    .ResourceContainer = *m_ResourceContainer,
                };

                messageDispatcher.Dispatch(context);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!OSMessageDispatch())
                {
                    return;
                }
            }

            if (deviceContext->BeginDraw())
            {
                isAnyActive = commandList->Execute(*deviceContext);

                const auto endDrawResult = deviceContext->EndDraw();
                const auto presentResult = renderTarget->Present();

                if (endDrawResult == D2DERR_RECREATE_TARGET || presentResult == DXGI_ERROR_DEVICE_REMOVED || presentResult == DXGI_ERROR_DEVICE_RESET)
                {
                    // デバイスロスト発生。リソースをクリーンアップしてコンテキストを再構築する
                    deviceContext = std::make_unique<Device::Context>();
                    renderTarget  = std::make_unique<Device::RenderTarget>(deviceContext.get(), renderTarget->GetTargetWindow());
                    deviceContext->SetTarget(*renderTarget);
                }
            }

            diagnosticsReporter.Submit(m_DiagnosticsSink);
        }
    }

} // namespace N503::Renderer2D
