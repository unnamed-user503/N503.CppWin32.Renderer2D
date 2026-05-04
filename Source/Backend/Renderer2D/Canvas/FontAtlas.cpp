#include "Pch.hpp"
#include "FontAtlas.hpp"

#include <stdexcept>

namespace N503::Renderer2D::Canvas
{
    auto FontAtlas::Create(ID2D1DeviceContext3* dc, IDWriteFactory3* dwFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset)
        -> std::unique_ptr<FontAtlas>
    {
        auto atlas = std::unique_ptr<FontAtlas>(new FontAtlas());

        // ---- オフスクリーン Bitmap を作成 ----
        // D2D1_BITMAP_OPTIONS_TARGET    : DC のレンダーターゲットとして使用可
        // CANNOT_DRAW を付けない        : DrawSpriteBatch のソースとして使用可
        D2D1_BITMAP_PROPERTIES1 props{};
        props.pixelFormat = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        props.dpiX = props.dpiY = 96.0f;
        props.bitmapOptions     = D2D1_BITMAP_OPTIONS_TARGET; // ★ CANNOT_DRAW を除去

        wil::com_ptr<ID2D1Bitmap1> stagingBitmap;
        dc->CreateBitmap({ AtlasWidth, AtlasHeight }, nullptr, 0, &props, &stagingBitmap);

        // ---- DC をオフスクリーンにバインドして描画 ----
        dc->SetTarget(stagingBitmap.get());
        dc->BeginDraw();
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));

        wil::com_ptr<ID2D1SolidColorBrush> brush;
        dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

        // ---- DirectWrite フォントメトリクス取得 ----
        DWRITE_FONT_METRICS1 fontMetrics{};
        fontFace->GetMetrics(&fontMetrics);

        const float scale   = emSize / static_cast<float>(fontMetrics.designUnitsPerEm);
        atlas->m_LineHeight = (fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * scale;

        float cursorX   = 1.0f;
        float cursorY   = 1.0f;
        float rowHeight = 0.0f;

        for (const char32_t cp : charset)
        {
            // codepoint → glyph index
            UINT32 ucp        = static_cast<UINT32>(cp);
            UINT16 glyphIndex = 0;
            fontFace->GetGlyphIndices(&ucp, 1, &glyphIndex);

            // グリフメトリクス (デザイン単位)
            DWRITE_GLYPH_METRICS gm{};
            fontFace->GetDesignGlyphMetrics(&glyphIndex, 1, &gm, FALSE);

            const float glyphW   = gm.advanceWidth * scale;
            const float glyphH   = static_cast<float>(fontMetrics.ascent + fontMetrics.descent) * scale;
            const float bearingX = gm.leftSideBearing * scale;
            const float bearingY = static_cast<float>(fontMetrics.ascent) * scale;

            // 行折り返し
            if (cursorX + glyphW + 1.0f > static_cast<float>(AtlasWidth))
            {
                cursorX    = 1.0f;
                cursorY   += rowHeight + 1.0f;
                rowHeight  = 0.0f;
            }

            rowHeight = std::max(rowHeight, glyphH);

            // DirectWrite でグリフをアトラスにレンダリング
            DWRITE_GLYPH_RUN run{};
            run.fontFace     = fontFace;
            run.fontEmSize   = emSize;
            run.glyphCount   = 1;
            run.glyphIndices = &glyphIndex;

            // ★ bearingX は加算 (左ベアリング分だけカーソルから右にオフセット)
            const D2D1_POINT_2F baseline{ cursorX + bearingX, cursorY + bearingY };
            dc->DrawGlyphRun(baseline, &run, brush.get());

            atlas->m_Glyphs[cp] = GlyphInfo{
                .SourceRect = D2D1::RectU(
                    static_cast<UINT32>(cursorX), // セル左端
                    static_cast<UINT32>(cursorY),
                    static_cast<UINT32>(cursorX + glyphW), // advanceWidth 分の幅
                    static_cast<UINT32>(cursorY + glyphH)
                ),
                .AdvanceWidth = glyphW,
                .BearingX     = bearingX,
                .BearingY     = bearingY,
            };

            cursorX += glyphW + 1.0f; // 1px パディング (グリフ滲み防止)
        }

        dc->EndDraw();
        dc->SetTarget(nullptr); // バインド解除

        atlas->m_Bitmap = std::move(stagingBitmap);
        return atlas;
    }

    auto FontAtlas::GetGlyph(char32_t cp) const -> const GlyphInfo*
    {
        const auto it = m_Glyphs.find(cp);
        return (it != m_Glyphs.end()) ? &it->second : nullptr;
    }

} // namespace N503::Renderer2D::Canvas
