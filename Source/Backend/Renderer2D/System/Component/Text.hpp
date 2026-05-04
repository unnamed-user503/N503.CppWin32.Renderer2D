#pragma once

// 1. Project Headers
#include "../../Canvas/FontAtlas.hpp"

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

        RenderGroup Group{ RenderGroup::Text };

        Canvas::FontAtlas* Atlas{ nullptr };

        float LetterSpacing = 0.0f;

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            Content.clear();
            FontName.clear();
            FontSize      = 0.0f;
            Group         = RenderGroup::Text;
            Atlas         = nullptr;
            LetterSpacing = 0.0f;
            IsDirty       = true;
        }
    };

} // namespace N503::Renderer2D::System::Component
