#pragma once

// 1. Project Headers
#include "../../Canvas/Font/Atlas.hpp"

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

namespace N503::Renderer2D::System::Component
{

    struct Text
    {
        std::wstring Content;

        std::wstring FontName;

        float FontSize{ 0.0f };

        RenderGroup Group{ RenderGroup::Text };

        Canvas::Font::Atlas* Atlas{ nullptr };

        float LetterSpacing{ 0.0f };

        // 0.0f の場合は折り返しなし
        float WrapWidth{ 720.0f };

        // 行間倍率 (1.0f = ラインハイト等倍, 1.5f = 1.5倍)
        float LineHeightScale{ 1.0f };

        std::int32_t VisibleCount{ 0 };

        bool IsDirty{ true };

        auto Reset() noexcept -> void
        {
            Content.clear();
            FontName.clear();
            FontSize        = 0.0f;
            Group           = RenderGroup::Text;
            Atlas           = nullptr;
            LetterSpacing   = 0.0f;
            WrapWidth       = 0.0f;
            LineHeightScale = 1.0f;
            VisibleCount    = 0;
            IsDirty         = true;
        }
    };

} // namespace N503::Renderer2D::System::Component
