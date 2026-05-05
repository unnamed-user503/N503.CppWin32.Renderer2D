#pragma once

// 1. Project Headers
#include "Entity.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>

// Declaration
namespace N503::Renderer2D::System
{

    class TimeSystem
    {
    public:
        TimeSystem();

        auto Reset() -> void;

        auto Update() -> float;

    private:
        using Clock = std::chrono::high_resolution_clock;

        using TimePoint = std::chrono::time_point<Clock>;

        TimePoint m_BaseTime{};

        TimePoint m_PrevTime{};

        float m_DeltaTime{};

        float m_TotalTime{};
    };

} // namespace N503::Renderer2D::System
