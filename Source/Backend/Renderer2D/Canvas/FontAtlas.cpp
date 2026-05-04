#include "Pch.hpp"
#include "FontAtlas.hpp"

#include <stdexcept>

namespace N503::Renderer2D::Canvas
{
    auto FontAtlas::Create(
        ID2D1DeviceContext3* dc,
        IDWriteFactory3*     dwFactory,
        IDWriteFontFace3*    fontFace,
        float                emSize,
        std::u32string_view  charset
    ) -> std::unique_ptr<FontAtlas>
    {
        auto atlas = std::unique_ptr<FontAtlas>(new FontAtlas());

        // ---- オフスクリーン Bitmap を作成 ----
        // D2D1_BITMAP_OPTIONS_TARGET : DC のレンダーターゲットとして使用可
        // CANNOT_DRAW を付けない     : DrawSpriteBatch のソースとして使用可
        D2D1_BITMAP_PROPERTIES1 props{};
        props.pixelFormat   = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        props.dpiX          = props.dpiY = 96.0f;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;

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

            const float advanceWidth = gm.advanceWidth * scale;
            const float bearingX     = gm.leftSideBearing * scale;
            const float bearingY     = static_cast<float>(fontMetrics.ascent) * scale;

            // セル高さ = ascent + descent + lineGap で余裕を持たせる
            // さらに ceil で切り上げてサブピクセル描画の切れを防ぐ
            const float cellH = std::ceil(
                static_cast<float>(fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * scale
            );

            // 行折り返し (advanceWidth ベース、2px パディング込み)
            if (cursorX + advanceWidth + 2.0f > static_cast<float>(AtlasWidth))
            {
                cursorX   = 1.0f;
                cursorY  += rowHeight + 2.0f; // 垂直方向も 2px パディング
                rowHeight = 0.0f;
            }

            rowHeight = std::max(rowHeight, cellH);

            // DirectWrite でグリフをアトラスにレンダリング
            // baseline.x = cursorX + bearingX: DirectWrite の自然な描画位置
            DWRITE_GLYPH_RUN run{};
            run.fontFace     = fontFace;
            run.fontEmSize   = emSize;
            run.glyphCount   = 1;
            run.glyphIndices = &glyphIndex;

            const D2D1_POINT_2F baseline{ cursorX + bearingX, cursorY + bearingY };
            dc->DrawGlyphRun(baseline, &run, brush.get());

            // SourceRect = advanceWidth 幅のセル全体
            // BearingX の空白も含むため、描画側では penX のみで位置が決まる
            // right/bottom は ceil で切り上げ (float→uint 切り捨てによるサンプリング漏れ防止)
            atlas->m_Glyphs[cp] = Glyph{
                .SourceRect   = D2D1::RectU(
                                    static_cast<UINT32>(std::floor(cursorX)),
                                    static_cast<UINT32>(std::floor(cursorY)),
                                    static_cast<UINT32>(std::ceil(cursorX + advanceWidth)),
                                    static_cast<UINT32>(std::ceil(cursorY + cellH))
                                ),
                .AdvanceWidth = advanceWidth,
                .BearingX     = bearingX,
                .BearingY     = bearingY,
            };

            cursorX += advanceWidth + 2.0f; // 2px パディング (グリフ間のサンプリング漏れ防止)
        }

        dc->EndDraw();
        dc->SetTarget(nullptr); // バインド解除

        atlas->m_Bitmap = std::move(stagingBitmap);
        return atlas;
    }

    auto FontAtlas::GetGlyph(char32_t cp) const -> const Glyph*
    {
        const auto it = m_Glyphs.find(cp);
        return (it != m_Glyphs.end()) ? &it->second : nullptr;
    }

} // namespace N503::Renderer2D::Canvas
