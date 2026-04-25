#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <windows.h>

// 6. C++ Standard Libraries

namespace N503::Renderer2D::Device
{
    class Context;
}

namespace N503::Renderer2D::Device::Commands
{

    struct DrawBitmap
    {
        ResourceHandle ResourceHandle{};

        wil::com_ptr<ID2D1Bitmap1> Bitmap{ nullptr };

        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
