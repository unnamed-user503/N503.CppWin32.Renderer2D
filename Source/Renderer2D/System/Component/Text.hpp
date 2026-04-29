#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1.h>
#include <dwrite.h>

// 6. C++ Standard Libraries
#include <string>

namespace N503::Renderer2D::System::Component
{

    struct Text
    {
        std::wstring Content;

        std::wstring FontName;

        float FontSize{ 0.0f };

        D2D1_COLOR_F Color;

        wil::com_ptr<IDWriteTextFormat> TextFormat;

        wil::com_ptr<IDWriteTextLayout> TextLayout;

        wil::com_ptr<ID2D1SolidColorBrush> Brush;

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            Content.clear();
            FontName.clear();
            FontSize = 0.0f;

            TextFormat.reset();
            TextLayout.reset();
            Brush.reset();

            IsDirty = false;
        }
    };

}
