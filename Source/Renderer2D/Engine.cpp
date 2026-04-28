#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Device/Context.hpp"
#include "Device/RenderTarget.hpp"
#include "Device/Viewport.hpp"
#include "Message/Context.hpp"
#include "Message/Dispatcher.hpp"
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"
#include "System/Registry.hpp"
#include "System/RendererSystem.hpp"
#include "System/SpriteSystem.hpp"
#include "System/TextSystem.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <chrono>
#include <format>
#include <iostream>
#include <memory>
#include <semaphore>
#include <stop_token>
#include <thread>

namespace N503::Renderer2D
{

    auto Engine::GetInstance() noexcept -> Engine&
    {
        static Engine instance;
        return instance;
    }

    Engine::Engine()
    {
        m_MessageQueue   = std::make_unique<Message::Queue>();
        m_SystemRegistry = std::make_unique<System::Registry>();
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
                // m_StartedEvent.SetEvent();

                // COM を初期化する
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);

                // PeekMessageWを呼び出して、レンダラースレッドのメッセージキューを作成する
                MSG msg{};
                ::PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

                // レンダラースレッドを開始する
                Run(stopToken);

                // レンダラー スレッドが停止したことを示すイベントをリセット状態にする
                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        // スレッドが起動して初期化が完了するまで待機する
        signal.acquire();

        // レンダラースレッドが描画可能な状態になるまで待機する
        m_StartedEvent.wait();

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

        auto resources     = std::make_unique<Resource::Container>();
        auto deviceContext = std::make_unique<Device::Context>();

        auto spriteSystem   = std::make_unique<System::SpriteSystem>();
        auto textSystem     = std::make_unique<System::TextSystem>();
        auto rendererSystem = std::make_unique<System::RendererSystem>();

        std::unique_ptr<Device::RenderTarget> renderTarget = nullptr;

        // メッセージ ディスパッチャーを初期化する
        Message::Dispatcher messageDispatcher;

        // レンダラースレッドの終了時にリソースをクリーンアップするためのスコープ ガード
        auto cleanupResources = wil::scope_exit(
            [&]
            {
                spriteSystem.reset();
                textSystem.reset();
                rendererSystem.reset();

                m_SystemRegistry.reset();

                renderTarget.reset();
                deviceContext.reset();
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
        auto isAnyActive = true;

        while (!stopToken.stop_requested())
        {
            HWND hwnd = nullptr;

            if (renderTarget == nullptr && (hwnd = Device::Viewport::GetInstance().GetRenderTargetWindow()))
            {
                renderTarget = std::make_unique<Device::RenderTarget>(*deviceContext, hwnd);
                deviceContext->SetRenderTarget(renderTarget.get());
                m_StartedEvent.SetEvent();
            }

            // メッセージキューのイベントハンドルとOSメッセージの両方を待機する
            const auto count   = static_cast<DWORD>(waitHandles.size());
            const auto handles = waitHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                Message::Context context{
                    .ResourceContainer = *resources,
                    .Registry          = *m_SystemRegistry,
                    .DeviceContext     = *deviceContext,
                };
                messageDispatcher.Dispatch(*m_MessageQueue, context);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!OSMessageDispatch())
                {
                    return;
                }
            }

            // コマンドリストにコマンドが存在する場合は、デバイスコンテキストを使用してコマンドを実行し、レンダリングターゲットを提示します。
            if (deviceContext->BeginDraw({ 0.1f, 0.2f, 0.4f, 1.0f }))
            {
                auto start = std::chrono::steady_clock::now();

                spriteSystem->Update(*m_SystemRegistry, *deviceContext, *resources);
                rendererSystem->Update(*m_SystemRegistry, *deviceContext);
                textSystem->Update(*m_SystemRegistry, *deviceContext);

                auto end = std::chrono::steady_clock::now();
                std::cout << "[Renderer2D] <Profile> : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us\n";

                const auto endDrawResult = deviceContext->EndDraw();
                const auto presentResult = renderTarget->Present();

                if (endDrawResult == D2DERR_RECREATE_TARGET || presentResult == DXGI_ERROR_DEVICE_REMOVED || presentResult == DXGI_ERROR_DEVICE_RESET)
                {
                    // デバイスが削除されたかリセットされた場合は、デバイス コンテキストとレンダリング ターゲットを再作成する
                    deviceContext = std::make_unique<Device::Context>();
                    renderTarget  = std::make_unique<Device::RenderTarget>(*deviceContext, renderTarget->GetTargetWindow());
                }
            }

            diagnosticsReporter.Submit(m_DiagnosticsSink);
        }
    }

} // namespace N503::Renderer2D
