#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{

    class Viewport
    {
    public:
        static auto GetInstance() noexcept -> Viewport&;

    public:
        auto GetRenderTargetWindow() const noexcept -> HWND;
    };

} // namespace N503::Renderer2D::Device
