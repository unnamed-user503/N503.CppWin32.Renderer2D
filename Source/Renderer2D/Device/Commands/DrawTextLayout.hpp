#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Renderer2D/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::Device
{
    class Context;
}

#undef DrawText

namespace N503::Renderer2D::Device::Commands
{

    struct DrawTextLayout
    {
        std::string Text;

        PointF Destination;

        struct
        {
            std::string Family;

            float Size{};
        }
        Font;

        wil::com_ptr<IDWriteTextLayout> Layout;

        wil::com_ptr<IDWriteTextFormat> Format;

        wil::com_ptr<ID2D1SolidColorBrush> Brush;

        auto Execute(Context& context) -> void;
    };

} // namespace N503::Renderer2D::Device::Commands
