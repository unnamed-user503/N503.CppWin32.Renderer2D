#include "Pch.hpp"
#include "FontAtlas.hpp"
#include <stdexcept>

namespace N503::Renderer2D::Canvas
{
    auto FontAtlas::Create(ID2D1DeviceContext3* dc, IDWriteFactory3* dwFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset)
        -> std::unique_ptr<FontAtlas>
    {
        auto atlas = std::unique_ptr<FontAtlas>(new FontAtlas());

        // --- オフスクリーン Bitmap (CPU書き込み可) を作成 ---
        D2D1_BITMAP_PROPERTIES1 props{};
        props.pixelFormat = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        props.dpiX = props.dpiY = 96.0f;
        props.bitmapOptions     = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        wil::com_ptr<ID2D1Bitmap1> stagingBitmap;
        dc->CreateBitmap({ AtlasWidth, AtlasHeight }, nullptr, 0, &props, &stagingBitmap);

        // --- DC をオフスクリーンにバインドして描画 ---
        dc->SetTarget(stagingBitmap.get());
        dc->BeginDraw();
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));

        wil::com_ptr<ID2D1SolidColorBrush> brush;
        dc->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

        // --- DWRITE メトリクス取得 ---
        DWRITE_FONT_METRICS1 fontMetrics{};
        fontFace->GetMetrics(&fontMetrics);

        const float scale   = emSize / static_cast<float>(fontMetrics.designUnitsPerEm);
        atlas->m_LineHeight = (fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * scale;

        float cursorX = 1.0f, cursorY = 1.0f;
        float rowHeight = 0.0f;

        for (char32_t cp : charset)
        {
            // codepoint → glyph index
            UINT32 ucp = static_cast<UINT32>(cp);
            UINT16 glyphIndex{};
            fontFace->GetGlyphIndices(&ucp, 1, &glyphIndex);

            // グリフメトリクス
            DWRITE_GLYPH_METRICS gm{};
            fontFace->GetDesignGlyphMetrics(&glyphIndex, 1, &gm, FALSE);

            const float glyphW   = gm.advanceWidth * scale;
            const float glyphH   = (gm.verticalOriginY > 0 ? static_cast<float>(fontMetrics.ascent + fontMetrics.descent) : emSize) * scale;
            const float bearingX = gm.leftSideBearing * scale;
            const float bearingY = static_cast<float>(fontMetrics.ascent) * scale;

            // 行折り返し
            if (cursorX + glyphW + 1.0f > AtlasWidth)
            {
                cursorX    = 1.0f;
                cursorY   += rowHeight + 1.0f;
                rowHeight  = 0.0f;
            }

            rowHeight = std::max(rowHeight, glyphH);

            // --- DirectWrite でグリフをアトラスに直接レンダリング ---
            DWRITE_GLYPH_RUN run{};
            run.fontFace     = fontFace;
            run.fontEmSize   = emSize;
            run.glyphCount   = 1;
            run.glyphIndices = &glyphIndex;

            // ベースライン位置を合わせる
            D2D1_POINT_2F baseline{ cursorX - bearingX, cursorY + bearingY };
            dc->DrawGlyphRun(baseline, &run, brush.get());

            atlas->m_Glyphs[cp] = GlyphInfo{
                .SourceRect   = D2D1::RectU(cursorX, cursorY, cursorX + glyphW, cursorY + glyphH),
                .AdvanceWidth = glyphW,
                .BearingX     = bearingX,
                .BearingY     = bearingY,
            };

            cursorX += glyphW + 1.0f; // 1px パディング（グリフ滲み防止）
        }

        dc->EndDraw();
        dc->SetTarget(nullptr); // バインド解除

        atlas->m_Bitmap = std::move(stagingBitmap);
        return atlas;
    }

    auto FontAtlas::GetGlyph(char32_t cp) const -> const GlyphInfo*
    {
        auto it = m_Glyphs.find(cp);
        return (it != m_Glyphs.end()) ? &it->second : nullptr;
    }
} // namespace N503::Renderer2D::Canvas
