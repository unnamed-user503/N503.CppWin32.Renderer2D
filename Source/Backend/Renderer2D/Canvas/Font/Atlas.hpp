#pragma once

// 1. Project Headers
#include "Glyph.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>

// Declaration
namespace N503::Renderer2D::Canvas::Font
{

    class Atlas
    {
    public:
        static constexpr uint32_t AtlasWidth = 2048;

        static constexpr uint32_t AtlasHeight = 2048;

        Atlas(ID2D1DeviceContext3* deviceContext, IDWriteFactory3* dwriteFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset = U"");

        [[nodiscard]]
        auto GetGlyph(char32_t codePage) const -> const Glyph*;

        [[nodiscard]]
        auto GetBitmap() const -> ID2D1Bitmap1*
        {
            return m_Bitmap.get();
        }

        [[nodiscard]]
        auto GetLineHeight() const -> float
        {
            return m_LineHeight;
        }

    private:
        wil::com_ptr<ID2D1Bitmap1> m_Bitmap;

        std::unordered_map<char32_t, Glyph> m_Glyphs;

        float m_LineHeight = 0.0f;

        std::u32string m_Charset;
    };

} // namespace N503::Renderer2D::Canvas::Font
