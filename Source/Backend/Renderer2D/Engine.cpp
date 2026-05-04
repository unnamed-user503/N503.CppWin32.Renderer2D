#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Canvas/Device.hpp"
#include "Canvas/Session.hpp"
#include "Canvas/Surface.hpp"
#include "Canvas/Window.hpp"

#include "Message/Context.hpp"
#include "Message/Dispatcher.hpp"
#include "Message/Queue.hpp"
#include "Resource/Container.hpp"
#include "System/Registry.hpp"
#include "System/RendererSystem.hpp"
#include "System/SpriteSystem.hpp"
#include "System/TextSystem.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

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
        m_MessageQueue        = std::make_unique<Message::Queue>();
        m_SystemRegistry      = std::make_unique<System::Registry>();
        m_DiagnosticsReporter = std::make_unique<Diagnostics::Reporter>();
        m_DiagnosticsReporter->AddSink(std::make_unique<Diagnostics::DebugStringSink>());
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

                // COM を初期化する
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);

                // PeekMessageWを呼び出して、レンダラースレッドのメッセージキューを作成する
                MSG msg{};
                ::PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

                // レンダラー スレッドの初期化が完了したことを通知する
                signal.release();

                // エンジンが開始されたことを示すイベントをシグナル状態にする
                m_IsRunning.store(true, std::memory_order_release);

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
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_RendererThread.native_handle()), WM_QUIT, 0, 0))
        {
            m_DiagnosticsReporter->Error(
                std::format(L"PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_RendererThread.native_handle()).data()
            );
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
        auto resources    = std::make_unique<Resource::Container>();
        auto canvasDevice = std::make_unique<Canvas::Device>();

        auto spriteSystem   = std::make_unique<System::SpriteSystem>();
        auto textSystem     = std::make_unique<System::TextSystem>();
        auto rendererSystem = std::make_unique<System::RendererSystem>();

        std::unique_ptr<Canvas::Surface> canvasSurface = nullptr;

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

                canvasSurface.reset();
                canvasDevice.reset();
            }
        );

        // OS メッセージをディスパッチするラムダ式
        auto ProcessMessage = []() -> bool
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

        std::chrono::steady_clock::time_point m_lastLogTime = std::chrono::steady_clock::now();
        std::vector<long long> m_durations;

        while (!stopToken.stop_requested())
        {
            HWND hwnd = nullptr;

            if (canvasSurface == nullptr && (hwnd = Device::Window::FindRenderTargetWindow()))
            {
                canvasSurface = std::make_unique<Canvas::Surface>(*canvasDevice, hwnd);
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
                    .CanvasDevice      = *canvasDevice,
                };
                messageDispatcher.Dispatch(*m_MessageQueue, context);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!ProcessMessage())
                {
                    return;
                }
            }

            if (canvasSurface)
            {
                auto canvasSession = canvasSurface->Begin({ 0.1f, 0.2f, 0.4f, 1.0f });

                auto start = std::chrono::steady_clock::now();

                spriteSystem->Update(*m_SystemRegistry, *canvasDevice, *resources);
                textSystem->Update(*m_SystemRegistry);
                rendererSystem->Update(*m_SystemRegistry, canvasSession);

                auto end = std::chrono::steady_clock::now();

                // 今回の経過時間(us)を保存
                auto elapsedUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
                m_durations.push_back(elapsedUs);

                // 前回のログ出力から16ms以上経過したかチェック
                auto now              = std::chrono::steady_clock::now();
                auto timeSinceLastLog = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastLogTime).count();

                if (timeSinceLastLog >= 1000 && !m_durations.empty())
                {
                    // 合計を計算
                    long long sum = 0;
                    for (auto d : m_durations)
                    {
                        sum += d;
                    }

                    double average = static_cast<double>(sum) / m_durations.size();

                    // 蓄積されたデータ数がそのままFPS（1秒あたりのフレーム数）になる
                    size_t fps = m_durations.size();

                    m_DiagnosticsReporter->Verbose(std::format("<Profile> Avg: {:.2f} us | FPS: {} | Total Frames: {}", average, fps, fps));

                    m_durations.clear();
                    m_lastLogTime = now;
                }

                // m_DiagnosticsReporter->Verbose(std::format("<Profile> : {} us", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()));

                const auto endDrawResult = canvasSession.End();
                const auto presentResult = canvasSurface->Present();

                if (endDrawResult == D2DERR_RECREATE_TARGET || presentResult == DXGI_ERROR_DEVICE_REMOVED || presentResult == DXGI_ERROR_DEVICE_RESET)
                {
                    canvasDevice  = std::make_unique<Canvas::Device>();
                    canvasSurface = std::make_unique<Canvas::Surface>(*canvasDevice, canvasSurface->GetTargetWindow());
                }
            }

            m_DiagnosticsReporter->Report();
        }
    }

} // namespace N503::Renderer2D
