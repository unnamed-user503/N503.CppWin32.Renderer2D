#include "Pch.hpp"
#include "Atlas.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <stdexcept>

namespace N503::Renderer2D::Canvas::Font
{

    Atlas::Atlas(ID2D1DeviceContext3* deviceContext, IDWriteFactory3* dwriteFactory, IDWriteFontFace3* fontFace, float emSize, std::u32string_view charset)
    {
        D2D1_BITMAP_PROPERTIES1 properties{};
        properties.pixelFormat   = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
        properties.dpiX          = 96.0f;
        properties.dpiY          = 96.0f;
        properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;

        wil::com_ptr<ID2D1Bitmap1> stagingBitmap;
        deviceContext->CreateBitmap({ AtlasWidth, AtlasHeight }, nullptr, 0, &properties, &stagingBitmap);
        deviceContext->SetTarget(stagingBitmap.get());
        deviceContext->BeginDraw();
        deviceContext->Clear(D2D1::ColorF(0, 0, 0, 0));

        wil::com_ptr<ID2D1SolidColorBrush> brush;
        deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brush);

        DWRITE_FONT_METRICS1 fontMetrics{};
        fontFace->GetMetrics(&fontMetrics);

        const float scale = emSize / static_cast<float>(fontMetrics.designUnitsPerEm);
        m_LineHeight      = (fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * scale;

        float cursorX   = 1.0f;
        float cursorY   = 1.0f;
        float rowHeight = 0.0f;

        for (const char32_t cp : charset)
        {
            UINT32 ucp        = static_cast<UINT32>(cp);
            UINT16 glyphIndex = 0;
            fontFace->GetGlyphIndices(&ucp, 1, &glyphIndex);

            DWRITE_GLYPH_METRICS gm{};
            fontFace->GetDesignGlyphMetrics(&glyphIndex, 1, &gm, FALSE);

            const float advanceWidth = gm.advanceWidth * scale;
            const float bearingX     = gm.leftSideBearing * scale;
            const float bearingY     = static_cast<float>(fontMetrics.ascent) * scale;

            const float cellH = std::ceil(static_cast<float>(fontMetrics.ascent + fontMetrics.descent + fontMetrics.lineGap) * scale);

            if (cursorX + advanceWidth + 2.0f > static_cast<float>(AtlasWidth))
            {
                cursorX    = 1.0f;
                cursorY   += rowHeight + 2.0f;
                rowHeight  = 0.0f;
            }

            rowHeight = std::max(rowHeight, cellH);

            DWRITE_GLYPH_RUN run{};
            run.fontFace     = fontFace;
            run.fontEmSize   = emSize;
            run.glyphCount   = 1;
            run.glyphIndices = &glyphIndex;

            const D2D1_POINT_2F baseline{ cursorX + bearingX, cursorY + bearingY };
            deviceContext->DrawGlyphRun(baseline, &run, brush.get());

            m_Glyphs[cp] = Glyph{
                .SourceRect = D2D1::RectU(
                    static_cast<UINT32>(std::floor(cursorX)),
                    static_cast<UINT32>(std::floor(cursorY)),
                    static_cast<UINT32>(std::ceil(cursorX + advanceWidth)),
                    static_cast<UINT32>(std::ceil(cursorY + cellH))
                ),
                .AdvanceWidth = advanceWidth,
                .BearingX     = bearingX,
                .BearingY     = bearingY,
            };

            cursorX += advanceWidth + 2.0f;
        }

        deviceContext->EndDraw();
        deviceContext->SetTarget(nullptr);

        m_Bitmap = std::move(stagingBitmap);
    }

    auto Font::Atlas::GetGlyph(char32_t cp) const -> const Glyph*
    {
        const auto it = m_Glyphs.find(cp);
        return (it != m_Glyphs.end()) ? &it->second : nullptr;
    }

} // namespace N503::Renderer2D::Canvas::Font
