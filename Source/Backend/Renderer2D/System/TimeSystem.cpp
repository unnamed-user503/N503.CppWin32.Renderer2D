#include "Pch.hpp"
#include "TimeSystem.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>

// Definition
namespace N503::Renderer2D::System
{

    TimeSystem::TimeSystem()
        : m_DeltaTime(0.0f)
        , m_TotalTime(0.0f)
    {
        Reset();
    }

    auto TimeSystem::Reset() -> void
    {
        m_BaseTime = Clock::now();
        m_PrevTime = m_BaseTime;
        m_DeltaTime = 0.0f;
        m_TotalTime = 0.0f;
    }

    auto TimeSystem::Update() -> float
    {
        auto currentTime = Clock::now();

        // 前フレームからの経過時間を計算
        std::chrono::duration<float> elapsed = currentTime - m_PrevTime;
        m_DeltaTime = elapsed.count();

        // 蓄積された総時間を計算
        std::chrono::duration<float> total = currentTime - m_BaseTime;
        m_TotalTime = total.count();

        // 次の計算のために現在の時間を保存
        m_PrevTime = currentTime;

        // スパイク（極端な処理落ち）対策: 
        // 1フレームが 0.1秒(10fps相当)を超えた場合は、計算が破綻しないようクランプする
        if (m_DeltaTime > 0.1f)
        {
            m_DeltaTime = 0.1f;
        }

        return m_DeltaTime;
    }
}
