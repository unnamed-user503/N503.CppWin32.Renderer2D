// FontAtlas.hpp
#pragma once
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

#include <wil/com.h>

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace N503::Renderer2D::Canvas
{
    struct Glyph
    {
        D2D1_RECT_U SourceRect;

        float AdvanceWidth;

        float BearingX;

        float BearingY;
    };

    class FontAtlas
    {
    public:
        static constexpr std::u32string_view BasicLatin = U" !\"#$%&'()*+,-./0123456789:;<=>?"
                                                          U"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                                          U"`abcdefghijklmnopqrstuvwxyz{|}~";

        static constexpr uint32_t AtlasWidth  = 1024;

        static constexpr uint32_t AtlasHeight = 1024;

        static auto Create(ID2D1DeviceContext3* deviceContext, IDWriteFactory3* dwriteFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset) -> std::unique_ptr<FontAtlas>;

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
    };
} // namespace N503::Renderer2D::Canvas
