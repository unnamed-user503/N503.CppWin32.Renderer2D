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
    struct GlyphInfo
    {
        D2D1_RECT_U SourceRect; // アトラス上のUV矩形（ピクセル座標）
        float AdvanceWidth;     // 次の文字までの水平距離
        float BearingX;         // 左側ベアリング
        float BearingY;         // ベースラインからの上端距離
    };

    class FontAtlas
    {
    public:
        static constexpr std::u32string_view BasicLatin = U" !\"#$%&'()*+,-./0123456789:;<=>?"
                                                          U"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                                                          U"`abcdefghijklmnopqrstuvwxyz{|}~";

        // アトラスサイズ・フォントサイズは用途に応じて調整
        static constexpr uint32_t AtlasWidth  = 1024;
        static constexpr uint32_t AtlasHeight = 1024;

        // DirectWrite/Direct2D リソースから生成
        static auto Create(
            ID2D1DeviceContext3* dc,
            IDWriteFactory3* dwFactory,
            IDWriteFontFace3* fontFace,
            float emSize,               // フォントサイズ (px)
            std::u32string_view charset // 焼き込む文字セット
        ) -> std::unique_ptr<FontAtlas>;

        [[nodiscard]] auto GetBitmap() const -> ID2D1Bitmap1*
        {
            return m_Bitmap.get();
        }
        [[nodiscard]] auto GetGlyph(char32_t cp) const -> const GlyphInfo*;
        [[nodiscard]] auto GetLineHeight() const -> float
        {
            return m_LineHeight;
        }

    private:
        wil::com_ptr<ID2D1Bitmap1> m_Bitmap;
        std::unordered_map<char32_t, GlyphInfo> m_Glyphs;
        float m_LineHeight = 0.0f;
    };
} // namespace N503::Renderer2D::Canvas
